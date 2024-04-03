# hoboNicola library for GK68 and FMV-KB611

![](assets/images/RP2040_adapter.jpg)

本ライブラリはオリジナルのhoboNicola libraryを特定のキーボード向けアダプター用に一部改変したものです。Arduinoのスケッチ部分は \hoboNicolaLibrary\examples にあります。

USキーボードを選択しない場合はKB611用のキー配列一択になり、JIS109キーボードの配列には合いません。

アダプターやキーボードの [【実際の写真】](./assets/hobonicola_gallery.md)

## SKYLOONG GK68 Lite / GK68X
	● 対象アダプター
	rp_hobo_nicola_GK68  : Seeed Stduio XIAO RP2040
	usb_hobo_nicola_GK68 : Seeduino XIAO-m1 + mini USB Host Shield

	設定モードに入るには 左CTRL + MENU(App) + 100ms -> 'S'キー
	● GK68X USキーボードの設定モード; *2 + *U + *M

	・GK68X B割れUSキーボード用に設定を固定。
	・左親指シフトキーはF14、右親指シフトキーはSpaceにキーボード側で設定する。
	--------------------------------------
	|      F14      |      SP     |  L2  |		Layer1
	--------------------------------------
	--------------------------------------
	|      TAB      |      F15    |  SP  |		Layer2
	--------------------------------------

	********************************************************
	*** CPU Seeed XIAO RP2040 (Raspberry Pi Pico/RP2040) ***
	*** CPU clock = 120MHz                               ***
	*** USB Stack = Adafruit TinyUSB                     ***
	********************************************************

	\作業ディレクトリ---+---\rp_hobo_nicola_GK68---rp_hobo_nicola_GK68.ino
	　                  |
	                    +---\libralies---+---\Adafruit_TinyUSB_Library
	                                     |
	                                     +---\hoboNicolaLibrary
	                                     |
	                                     +---\Pico_PIO_USB
## 富士通FMV-KB611
	● 対象アダプター
	ps2_hobo_nicola_KB611  : SparkFun Pro Micro相当品(+5V, 16MHz版)

	設定モードに入るには 右CTRL + MENU(App) + 100ms -> 'S'キー
	● FMV-KB611キーボードの設定モード; *3 + *N + *K

	---------------------------------
	|      F23      |      F24      |	親指シフトキーは内部で使うだけ。
	---------------------------------
            | 無変換 |  変換  |
	        ------------------

	********************************************************
	*** CPU Arduino Leonardo (Arduino AVR Boards)        ***
	********************************************************

	\作業ディレクトリ---+---\ps2_hobo_nicola_KB611---ps2_hobo_nicola_KB611.ino
	　                 |
	                   +---\libralies---+---\hoboNicolaLibrary
