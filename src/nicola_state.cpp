/**
 * 
	nicola_state.cpp   NICOLA-state-machine implements simultaneous typing method of "Hobo-nicola keyboard and adapter library".
	Copyright (c) 2018 Takeshi Higasa, okiraku-camera.tokyo

	This file is part of "Hobo-nicola keyboard and adapter library".

	"Hobo-nicola keyboard and adapter" is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by the Free Software Foundation, 
	either version 3 of the License, or (at your option) any later version.

	"Hobo-nicola keyboard and adapter" is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
	FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with "Hobo-nicola keyboard and adapter".  If not, see <http://www.gnu.org/licenses/>.

		Included in hoboNicola 1.5.0.		July. 31. 2021.
			based on nicola_keyboard.cpp
		hoboNicola 1.7.0. March.3. 2023
			Separating nicola tables to nicola_table.cpp
*/
//**koseki(2024.5.1)
//	- deleted the Release_Wait_State and Repeat_State.
//	- added output_oya_long_press() for long time pressing oyayubi.
//**

#include "hobo_nicola.h"
#include "hobo_settings.h"

void HoboNicola::timer_tick(unsigned long now) {
	if (event_time != 0 && (now > event_time)) {
		event_time = 0;
		nicola_state(Time_out, 0);
	}
}

void HoboNicola::moji_set(uint16_t param) {
	moji = param;
	moji_time = millis();
	event_time = moji_time + e_charTime;
}

void HoboNicola::oyayubi_set(uint16_t param) {
	oyayubi = param;
	oyayubi_time = millis();
	event_time = oyayubi_time + e_oyaTime;
}

// 即時出力用の前文字キャンセル。
#define sendBS() stroke(HID_BACKSP, 0, true)	

static bool immediate = false;

void HoboNicola::nicola_state(nicola_event_t e, uint16_t param) {
	unsigned long now = millis();
	switch(state) {
		case Initial_State:
			event_time = moji_time = oyayubi_time = 0;
			oyayubi = moji = 0;
			immediate = false;
			switch(e) {
				case Moji_pressed:
					moji_set(param);
					state = Character_State;
					break;
				case Oyayubi_pressed:
					oyayubi_set(param);
					state = Oyayubi_State;
					break;
				default:
					break;
			}
			break;
		case Character_State:
			switch(e) {
				case Init:
					state = Initial_State;
					break;
				case All_off:
					output();
					state = Initial_State;
					break;
				case Key_released:
					if (param == moji) {	// 関係のない文字のオフは無視する。
						output();
						state = Initial_State;
					}
					break;
				case Moji_pressed:
					output();
					moji_set(param);
					break;
				case Oyayubi_pressed:
					oyayubi_set(param);
					state = Char_Oya_State;
					break;
				case Time_out:
					output();
				//**koseki(2024.5.1)
					state = Initial_State;
				//**
					break;
				default:
					break;			
			}
			break;
		//**koseki(2024.5.1)
		//case Release_Wait_State:	// 長押しタイムアウト待ち
		//**
		case Oyayubi_State:
			switch(e) {
				case Init:
					state = Initial_State;
					break;
				case Moji_pressed:
					moji_set(param);
					output();
					state = Initial_State;
					break;
				case All_off:
					output();
					state = Initial_State;
					break;
				case Key_released:
					if (param == oyayubi) {
						output();
						state = Initial_State;
					}
					break;
				case Oyayubi_pressed:
					output();
					oyayubi_set(param);
					break;
				case Time_out:
					if (!dedicated_oyakeys) {
					//**koseki(2024.5.1)
						output_oya_long_press();
						state = Initial_State;
					//**
					}
					break;
				default:
					break;			
			}
			break;
		case Char_Oya_State:
			switch(e) {
				case Init:
					state = Initial_State;
					break;
				case All_off:
					output();
					state = Initial_State;
					break;
				case Oyayubi_pressed:
					output();
					oyayubi_set(param);
					state = Oyayubi_State;
					break;
				case Moji_pressed:
					if ((oyayubi_time - moji_time) < (now - oyayubi_time)) {
						output();   // 先行の文字と親指
						moji_set(param);
						state = Character_State;
					} else {
						uint16_t oya = oyayubi;
						oyayubi = 0;
						if (!immediate)
							output();   // 先行の文字だけ出す
						immediate = false;
						output();   // 後追いの文字と親指
					//**koseki(2024.5.1)
						state = Initial_State;
					//**
					}
					break;
				case Key_released:  // 文字キーが離されたタイミングに応じて単独打鍵とする。
					if ((moji == param) && (now - oyayubi_time < e_nicolaTime) && (oyayubi_time - moji_time) > (now - oyayubi_time)) {
						uint16_t oya = oyayubi;
						oyayubi = 0;
						if (!immediate)
							output();  
						moji = 0;
						immediate = false;
						oyayubi = oya;
						state = Oyayubi_State;
						break;
					}
					output();
					state = Initial_State;
					break;
				case Time_out:
					output();
				//**koseki(2024.5.1)
					state = Initial_State;
				//**
					break;
				default:
					break;		
			}
			break;
		//**koseki(2024.5.1)
		//case Repeat_State:
		//**
		default:
			state = Initial_State;
			break;
	}
}

#define isShiftPressed() (bool) ((modifiers & HID_SHIFT_MASK) != 0)

void HoboNicola::output() {
	if (moji == 0 && oyayubi) {
		stroke(HIGHBYTE(oyayubi), modifiers);
		oyayubi = 0;
		return;
	}
	if (immediate) {
		immediate = false;
		if (oyayubi)
			sendBS();
		else
			return;
	}
	if (setup_mode) {
		setup_options(LOWBYTE(moji));
		moji = 0;
		oyayubi = 0;
		return;
	}

	if (isShiftPressed() && oyayubi == 0)
		oyayubi = NID_SHIFT_KEY;

	const uint8_t* p = get_output_data(moji, oyayubi);
	oyayubi = 0;
	moji = 0;
	if (p) {
		/*
		if (state == Release_Wait_State) {
			sendBS();	// タイムアウトで出した文字をキャンセル。
			delay(2);
		}
		*/
		send_PGM_string(p);
	}
}

void HoboNicola::output_oya_long_press() {
	// USキーボードのときだけ親指キーの長押しが有効
	if (_US_LAYOUT(global_setting)) {
		if(LOWBYTE(oyayubi) == NID_LEFT_OYAYUBI) {
			stroke(HID_TAB, modifiers);         // タイムアウト時はTAB(変換候補選択)キー
		}
		else if(LOWBYTE(oyayubi) == NID_RIGHT_OYAYUBI) {
			stroke(HID_F15, modifiers);         // タイムアウト時はF15(変換)キー
		}
	}
	moji = 0;
	oyayubi = 0;
}

