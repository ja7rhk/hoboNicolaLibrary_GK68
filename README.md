# hoboNicola library for [SKYLOONG GK68](https://skyloongtech.com/skyloong-gk68/ref/13/)

![](assets/images/RP2040_adapter.jpg)

本ライブラリはオリジナルのhoboNicola libraryを特定のキーボード向けアダプター用に一部改変したものです。Arduinoのスケッチ部分は \hoboNicolaLibrary\examples にあります。

アダプターやキーボードの [【実際の写真】](./assets/hobonicola_gallery.md)

	● 対象アダプター
	rp_hobo_nicola_GK68  : Seeed Stduio XIAO RP2040
	usb_hobo_nicola_GK68 : Seeed Stduio XIAO SAMD21 + mini USB Host Shield

	設定モードに入るには 左CTRL + MENU(App) + 100ms -> 'S'キー
	● GK68X USキーボードの設定モード; *2 + *U + *N + *I

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

## observe_imeとMS-IMEの設定

アダプターを使用する場合、observe_imeを利用すると所謂モードずれがないので便利です。GK68は10キーがないのでNumLockでモード切替えをします。

![](assets/images/observe_ime_num.png)

USキーボードは無変換・変換キーがないのでF14/F15を使用します。以下はMS-IMEの設定例です(以前のバージョンを使用)。カナ変換はF7キーで出来ます。

![](assets/images/ms_ime.png)


## 情報元
* https://github.com/okiraku-camera/hoboNicolaLibrary オリジナルのhoboNicola library。 
* https://github.com/okiraku-camera/observe_ime hoboNicolaで使用しているobserve_imeの情報です。
* https://www.pcbway.com/project/shareproject/hoboNicola_for_XIAO_RP2040_85b65544.html プリント基板の入手先です。


