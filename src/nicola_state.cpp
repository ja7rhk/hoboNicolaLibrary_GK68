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
		case S1_Initial_State:
			event_time = moji_time = oyayubi_time = 0;
			oyayubi = moji = 0;
			immediate = false;
			switch(e) {
				case Moji_pressed:
					moji_set(param);
					state = S2_Character_State;
					break;
				case Oyayubi_pressed:
					oyayubi_set(param);
					state = S3_Oyayubi_State;
					break;
				default:
					break;
			}
			break;
		case S2_Character_State:
			switch(e) {
				case Init:
					state = S1_Initial_State;
					break;
				case All_off:
					output();
					state = S1_Initial_State;
					break;
				case Moji_pressed:
					output();
					moji_set(param);
					break;
				case Oyayubi_pressed:
					oyayubi_set(param);
					state = S4_Char_Oya_State;
					break;
				case Key_released:
					if (param == moji) {	// 関係のない文字のオフは無視する。
						output();
						state = S1_Initial_State;
					}
					break;
				case Time_out:
					output();
				//**koseki(2024.5.1)
					state = S1_Initial_State;
				//**
					break;
				default:
					break;			
			}
			break;
		//**koseki(2024.5.1)
		//case Release_Wait_State:	// 長押しタイムアウト待ち
		//**
		case S3_Oyayubi_State:
			switch(e) {
				case Init:
					state = S1_Initial_State;
					break;
				case All_off:
					output();
					state = S1_Initial_State;
					break;
				case Moji_pressed:
					moji_set(param);
					//**koseki(2024.5.1)
					//output();
					//state = S1_Initial_State;
					state = S5_Oya_Char_State;
					//**
					break;
				case Oyayubi_pressed:
					output();
					oyayubi_set(param);
					break;
				case Key_released:
					if (param == oyayubi) {
						output();
						state = S1_Initial_State;
					}
					break;
				case Time_out:
					if (!dedicated_oyakeys) {
					//**koseki(2024.5.1)
						output_oya_long_press();
						state = S1_Initial_State;
					//**
					}
					break;
				default:
					break;			
			}
			break;
		case S4_Char_Oya_State:
			switch(e) {
				case Init:
					state = S1_Initial_State;
					break;
				case All_off:
					output();
					state = S1_Initial_State;
					break;
				case Moji_pressed:
					//**koseki(2024.5.1)
				    // M ON --> O ON --> M OFF
					t1 = oyayubi_time - moji_time;
					t2 = now - oyayubi_time;
					if (t1 < t2) {
						output();   // 先行の文字と親指
						moji_set(param);
						state = S2_Character_State;
					} else {
						uint16_t oya = oyayubi;
						oyayubi = 0;
						output();   // 先行の文字だけ出す
						oyayubi = oya;
						moji_set(param);
						state = S5_Oya_Char_State;
					}
					break;
					//**
				case Oyayubi_pressed:
					output();
					oyayubi_set(param);
					state = S3_Oyayubi_State;
					break;
				case Key_released:  // 文字キーが離されたタイミングに応じて単独打鍵とする。
					//**koseki(2024.5.1)
	              	// M ON --> O ON --> M OFF
					t1 = oyayubi_time - moji_time;
					t2 = now - oyayubi_time;
					if ((moji == param) && (t2 < e_nicolaTime) && (t1 >= t2)) {
	                // M ON --> O ON --> M OFF (M is output, but O is still open to combo)
						uint16_t oya = oyayubi;
						oyayubi = 0;
						output();	// 先行の文字だけ出す
						moji = 0;
						oyayubi = oya;
						state = S3_Oyayubi_State;
						break;
					}
					output();
					state = S1_Initial_State;
					break;
				case Time_out:
					output();
					state = S1_Initial_State;
				//**
					break;
				default:
					break;		
			}
			break;
		//**koseki(2024.5.1)
		case S5_Oya_Char_State:
			switch(e) {
				case Init:
					state = S1_Initial_State;
					break;
				case All_off:
					output();
					state = S1_Initial_State;
					break;
				case Moji_pressed:
					output();   	// 先行の文字と親指
					moji_set(param);
					state = S2_Character_State;
					break;
				case Oyayubi_pressed:
					t1 = moji_time - oyayubi_time;
					t2 = now - moji_time;
					if (t1 < t2) {
						output();   // 先行の親指と文字
						oyayubi_set(param);
						state = S3_Oyayubi_State;
					} else {
						uint16_t mo = moji;
						moji = 0;
						output();   // 先行の親指だけ出す
						moji = mo;
						oyayubi_set(param);
						state = S4_Char_Oya_State;
					}
					break;
				case Key_released:  // 文字キーが離されたタイミングに応じて単独打鍵とする。
	              	// O ON --> M ON --> O OFF
					t1 = moji_time - oyayubi_time;
					t2 = now - moji_time;
					if ((oyayubi == param) && (t2 < e_nicolaTime) && (t1 >= t2)) {
					// O ON --> M ON --> O OFF (O is output, but M is still open to combo)
						uint16_t mo = moji;
						moji = 0;
						output();   // 先行の親指だけ出す  
						moji = mo;
						state = S2_Character_State;
						break;
					}
					output();
					state = S1_Initial_State;
					break;
				case Time_out:
					output();
					state = S1_Initial_State;
					break;
				default:
					break;		
			}
			break;
		//**
		//**koseki(2024.5.1)
		//case Repeat_State:
		//**
		default:
			state = S1_Initial_State;
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

