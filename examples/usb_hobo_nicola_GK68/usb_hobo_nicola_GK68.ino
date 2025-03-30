/**
	usb_hobo_nicola.ino Main sketch of "Hobo-nicola usb/ble adapter for MAX3421EE".
	Copyright (c) 2022-2023 Takeshi Higasa

	This file is part of "Hobo-nicola keyboard and adapter".

	"Hobo-nicola keyboard and adapter" is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by the Free Software Foundation, 
	either version 3 of the License, or (at your option) any later version.

	"Hobo-nicola keyboard and adapter" is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
	FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with "Hobo-nicola keyboard and adapter".  If not, see <http://www.gnu.org/licenses/>.

	version 1.7.6  Mar. 8, 2024.
		support Arduino boards below.
		Pro Micro(+3.3V/8MHz),
		QTPy-m0 and XIAO-m0, 
		SSCI ISP1807MB, XIAO-nRF52840,
		XIAO-RP2040, Adafruit KB2040,
*/

/**koseki(2024.3.20)

	設定モードに入るには 左CTRL + MENU(App) + 100ms -> 'S'キー
	GK68X USキーボードとUSBライトタッチキーボードを統合

	● GK68X USキーボードの設定モード; *2 + *U + *N
      ^^^^^^^^^^^^^^^^^
	・GK68X B割れUSキーボード用に設定を固定。
	・左親指シフトキーはF14、右親指シフトキーはSpaceにキーボード側で設定すること。
	--------------------------------------
	|      F14      |      SP     |  L2  |		Layer1
	--------------------------------------
	--------------------------------------
	|      DEL      |      F15    |      |		Layer2 (例)
	--------------------------------------
	・親指シフトキーを長押しすると以下のコードを送出する。
	-------------------------------
	|      TAB      |      F15    | 
	-------------------------------
	-------------------------------------------
	********************************************************
	*** CPU Seeeduino XIAO (Adafruits SAMD Boards)       ***
	*** USB Stack = TinyUSB                              ***
	********************************************************

	\作業ディレクトリ---+---\usb_hobo_nicola_GK68---usb_hobo_nicola_GK68.ino
	　                  |
	                    +---\libraries---+---\Adafruit_TinyUSB_Library
	                                     |
	                                     +---\hoboNicolaLibrary
										 
*/

#include "hobo_nicola.h"
#include "hobo_sleep.h"
#include "uhslib2.h"
#include "hobo_board_config.h"

#if defined(NRF52_SERIES)
static const uint8_t FN_BLE_SWITH  = FN_EXTRA_START;
#endif
// Function keys with Fn-key pressed.
static const uint16_t fn_keys[] PROGMEM = {
	//**koseki(2024.3.12)
	HID_S | WITH_R_CTRL,		FN_SETUP_MODE,
	HID_S | WITH_L_CTRL,		FN_SETUP_MODE,
	HID_7,						FN_MEDIA_SCAN_PREV,
	HID_8,						FN_MEDIA_PLAY_PAUSE,
	HID_9,						FN_MEDIA_SCAN_NEXT,
	HID_0,						FN_MEDIA_MUTE,
	HID_MINUS,					FN_MEDIA_VOL_DOWN, 
	HID_EQUALS,					FN_MEDIA_VOL_UP, 
	//**
	0, 0
};

class usb_hobo_nicola : public HoboNicola {
public:
	usb_hobo_nicola() {}
	const uint16_t* fn_keys_table() { return fn_keys; }
	// if you want to change soft-Fn key, un-comment below. 
	//  const uint8_t fn_key() { return HID_APP; }
	void extra_function(uint8_t fk, bool pressed);
};

void usb_hobo_nicola::extra_function(uint8_t k, bool pressed) {
	if (!pressed)
		return;
	switch (k) {
#if defined(NRF52_SERIES)
	case FN_BLE_SWITH:
		releaseAll();
		delay(10);
		if (is_ble_connected())
			stop_ble();
		else
			start_ble();
		break;
#endif
		default:
			break;
	}
}

usb_hobo_nicola hobo_nicola;

class KeyboardEvent : public KeyboardReportParser {
	USBHID *phid;
	uint8_t last_led;
	public:
		KeyboardEvent() {
			phid = 0;
			last_led = 0;
		};
		void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
			if (is_usb_suspended()) {
				usb_wakeup();
				delay(100);
				return;
			}
			phid = hid;
			KeyboardReportParser::Parse(hid, is_rpt_id, len, buf);
		};
		void sync_led( uint8_t led ) {
			if (phid && last_led != led) {
				phid->SetReport(0, 0, 2, 0, 1, &led);
				last_led = led;
			}
		};

	protected:
		void OnControlKeysChanged(uint8_t before, uint8_t after) {
			uint8_t change = after ^ before;
			if (!change) return;
			uint8_t mask = 1;
			uint8_t key = HID_MODIFIERS;
			for (uint8_t i = 0; i < HID_MODIFIERS_COUNT; i++, mask <<= 1)  {
				if (change & mask)  hobo_nicola.key_event(key + i, after & mask);
			}
		};
		void OnKeyDown  (uint8_t mod, uint8_t key) { hobo_nicola.key_event(key, true); };
		void OnKeyUp  (uint8_t mod, uint8_t key) { hobo_nicola.key_event(key, false); };
};

USBHost Usbhost;
USBHub Hub1(&Usbhost);
USBHub Hub2(&Usbhost);
USBHub Hub3(&Usbhost);
USBHub Hub4(&Usbhost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usbhost);
KeyboardEvent kbd;

void setup() {
	hobo_device_setup();
	HoboNicola::init_hobo_nicola(&hobo_nicola, "usb_hobo_nicola");
	HidKeyboard.SetReportParser(0, &kbd);
	if (Usbhost.Init() == -1)	hobo_nicola.error_blink();
	delay( 60 );
}

static const uint32_t hobo_sleep_ms = 1000;
// 30 minutes経過したらキーボードによる復帰はやらない。
static const uint32_t no_resume_ms = 1800L * 1000L; // 30minutes.
static const uint32_t no_resume_count = (no_resume_ms / hobo_sleep_ms);

uint32_t sleep_counter = 0;
bool deep_suspended = false;
bool suspended = false;
void loop() {
	if (is_usb_suspended() ) {
		kbd.sync_led(0);
		all_led_off();
		if (_USE_KBD_SUSPEND(global_setting)){
			Usbhost.suspendKeyboard(); // suspend keyboard and max3421E
			suspended = true;
		}
		enter_sleep(hobo_sleep_ms);	// suspend myself.
		if (sleep_counter > no_resume_count)
			deep_suspended = true;
		else if (!deep_suspended) {      
			sleep_counter++;
			if (suspended) {
				Usbhost.resumeKeyboard(); // resume keyboard and max3421E to poll keyboard
				suspended = false;      
			}
			Usbhost.Task();  // check any key pressed.
		} 
	} else {
		if (deep_suspended || suspended) {
			Usbhost.resumeKeyboard();
			delay(100);
			sleep_counter = 0;
			deep_suspended = false;
			suspended = false;      
		}    
		Usbhost.Task();
		hobo_nicola.idle();
		kbd.sync_led(HoboNicola::get_hid_led_state());
		ble_led(is_ble_connected());
		enter_sleep();
	}
}
