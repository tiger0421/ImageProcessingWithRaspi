# ライントレース
raspberrypiとカメラを用いて、リアルタイム画像処理によるライントレースをするためのプログラム

# Usage
```
$ python pyLineTrace.py
```

# Algorithm
以下のような15x12の画像を取得したときを考える。なお、黒い線がトレースするためのラインである。
![line1](https://github.com/tiger0421/raspi/blob/master/image/line1.png)

まずはじめに、処理範囲を限定する。下の画像では赤い部分(15x3)の領域を取得する。  
鉛直方向に1列ではなく3列とるのはメディアンフィルタを用いるからだ。  
取得した領域をHSVに変換し、黒い線のみを抽出できるようにマスクする。  
さらに、ノイズ除去のためにメディアンフィルタを今回は用いた。
![line1](https://github.com/tiger0421/raspi/blob/master/image/line2.png)

次に中点をとる。計算方法は単純に画像座標の平均を求める。  
今回の場合、黒いラインの幅は画像座標の(6,10)～(10,10)の範囲にまたがっているため、計算は、求めたい中点をaveXCoordinateとして  
aveXCoordinate = (6 + 7 + 8 + 9 +  10) / 5  
となる。
![line1](https://github.com/tiger0421/raspi/blob/master/image/line3.png)

以上の動作を繰り返し、中点をいくつか取得する。(以下の画像では3点)  
取得した中点を用いて最小二乗法を適用し、求めた傾きと切片からモータの出力を決定する。
![line1](https://github.com/tiger0421/raspi/blob/master/image/line4.png)

# License
MIT
