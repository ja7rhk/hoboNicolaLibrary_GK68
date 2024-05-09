# hoboNicola library for Skyloong GK68 Lite / GK68X

![](assets/images/RP2040_adapter.jpg)

本ライブラリはオリジナルのhoboNicola libraryを特定のキーボード向けアダプター用に一部改変したものです。Arduinoのスケッチ部分は \hoboNicolaLibrary\examples にあります。

アダプターやキーボードの [【実際の写真】](./assets/hobonicola_gallery.md)

	● 対象アダプター
	rp_hobo_nicola_GK68  : Seeed Stduio XIAO RP2040
	usb_hobo_nicola_GK68 : Seeed Stduio XIAO SAMD21 + mini USB Host Shield

	設定モードに入るには 左CTRL + MENU(App) + 100ms -> 'S'キー
	● GK68X USキーボードの設定モード; *2 + *U + *N

	・GK68X B割れUSキーボード用に設定を固定。
	・左親指シフトキーはF14、右親指シフトキーはSpaceにキーボード側で設定する。
	--------------------------------------
	|      F14      |      SP     |  L2  |		Layer1
	--------------------------------------
	・親指シフトキーを長押しすると以下のコードを送出する。
	-------------------------------
	|      TAB      |      F15    | 
	-------------------------------
	・レイヤー2への割り当てはキーボード側のドライバ(GK6XPlus Driver)で行う。
	--------------------------------------
	|      DEL      |      F15    |      |		Layer2 (例)
	--------------------------------------

## rp_hobo_nicola_GK68

	********************************************************
	*** CPU Seeed XIAO RP2040 (Raspberry Pi Pico/RP2040) ***
	*** CPU clock = 120MHz                               ***
	*** USB Stack = Adafruit TinyUSB                     ***
	********************************************************

	\作業ディレクトリ---+---\rp_hobo_nicola_GK68---rp_hobo_nicola_GK68.ino
	　                  |
	                    +---\libralies---+---\Adafruit_TinyUSB_Library
	                                     |
	                                     +---\hoboNicolaLibrary_GK68
	                                     |
	                                     +---\Pico_PIO_USB
## usb_hobo_nicola_GK68

	********************************************************
	*** Seeeduino XIAO (Adafruit SAMD Boards)            ***
	*** USB Stack = Adafruit TinyUSB                     ***
	********************************************************

	\作業ディレクトリ---+---\rp_hobo_nicola_GK68---usb_hobo_nicola_GK68.ino
	　                  |
	                    +---\libralies---+---\Adafruit_TinyUSB_Library
	                                     |
	                                     +---\hoboNicolaLibrary_GK68
