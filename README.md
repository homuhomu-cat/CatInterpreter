# Cat ^+_~^
## 記法
### 標準入出力
#### 入力
```
_ <-
```
入力に置換する．
#### 出力
```
<- A
```
Aが出力される．出力は必ず左端で行う．
### 引数
```
n -> function
```
```
n m -> function
```
n，m で function 内の文字を置換する．
### 条件分岐
#### 三項演算子
```
cond ? A : B
```
condが真(非ゼロ)ならA、そうでなければBに置換する．
### 実行
#### 上の行を実行
```
^ << args
```
#### 今の行を実行
```
. << args
```
### 組み込み関数
#### 加算
```
+ << A B
```
#### 減算
```
- << A B
```
#### 比較
```
> << A B
```
A > B ならば真．
#### 入替
```
~ << A B
```
#### 否定
```
! << A
```
## 記述例
### 加算
#### ソースコード
source/add.cats
```
<- + << _ <- _ <-
```
#### 入力例
```
2 3
```
#### 出力例
```
5
```
### 1からNまでの整数の和
#### ソースコード
source/sum.cats
```
S N -> N ? . << + << S N - << N 1 : S
<- ^ << 0 _ <-
```
#### 入力例
```
10
```
#### 出力例
```
55
```
