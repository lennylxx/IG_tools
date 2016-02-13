IG_tools
========
Tools to process the game files by Innocent Grey &amp; Noesis

igtool
------
extract and create the `*.dat` files used by Innocent Grey

### Supported Games
* Innocent Grey
  * Caucasus: Nanatsuki no Nie (クロウカシス - 七憑キノ贄)
  * PP pianissimo (ＰＰ-ピアニッシモ- 操リ人形ノ輪舞)
  * Cartagra ~Tsuki kurui no Yamai~ (カルタグラ～ツキ狂イノ病～)
  * Kara no Shoujo (殻ノ少女)
  * Nagomibako Innocent Grey Fandisc (和み匣 Innocent Greyファンディスク)
* Noesis
  * Furifure (フリフレ)

> For the games using `*.iga` as their archive format, try http://asmodean.reverse.net/pages/exiga.html.

igscript
--------
parse and create the `*.s` files extracted from script archive used by Innocent Grey

### Supported Games
* Innocent Grey
  * Caucasus: Nanatsuki no Nie (クロウカシス - 七憑キノ贄)
  * PP pianissimo (ＰＰ-ピアニッシモ- 操リ人形ノ輪舞)
  * Cartagra ~Tsuki kurui no Yamai~ (カルタグラ～ツキ狂イノ病～)
  * Kara no Shoujo (殻ノ少女)
  * Nagomibako Innocent Grey Fandisc (和み匣 Innocent Greyファンディスク)
  * Eisai Kyouiku (英才狂育)
  * Flowers -Le Volume sur Printemps- (FLOWERS 春篇)
  * Flowers -Le Volume sur Ete- (FLOWERS 夏篇)
* Noesis
  * Cure Girl
  * Furifure (フリフレ)
  * Furifure 2 (フリフレ2)

How to use
==========
1.use `igtool.exe` to extract script archive file.
```
igtool -x script.dat
```
2.enter the new `script~` folder and use `igscript.exe` to parse the script file.
```
igscript -p 00_0000.s 00_0000.txt
```
3.after `00_0000.txt` is edited, create the new script file.
```
igscript -c 00_0000.s 00_0000.txt new\00_0000.s
```
for `CureGirl` and `フリフレ2`, you need to add an extra option `-x`.
```
igscript -x -p 0000.s 0000.txt
igscript -x -c 0000.s 0000.txt new\0000.s
```
4.use `parseall.bat` and `createall.bat` to do batch job.

Script file (*.s) count table
-----------------------------
| Game Name      |Count|
|:---------------|----:|
|七憑キノ贄      | 1252|
|操リ人形ノ輪舞  |  174|
|殻ノ少女        |  601|
|ツキ狂イノ病    |  266|
|和み匣          |  138|
|CureGirl        |  413|
|フリフレ        |   50|
|フリフレ2       |   64|
|マーブル★ブルマ|   39|
|放課後の先パイ  |   24|
|英才狂育        |  345|

