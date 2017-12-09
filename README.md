# c_reversi

C言語を思い出すために書いてみたリバーシです。
アルファベータ法のようなアルゴリズムで、7手先まで読みます。
それっぽく確定石の計算や、12手前からの終盤の読み切りなどが実装されています。

以下の3種類の遊び方ができます。
- 黒:自分 vs 白:CPU
- 黒:CPU vs 白:自分
- 黒:CPU vs 白:乱択CPU

graphvizで探索やアルファベータ法の打ち切りが、可視化されています。[/dfs-visualize.svg](/dfs-visualize.svg)
![/dfs-visualize.svg](/dfs-visualize.svg)

`think.c` 先頭の `#define _GRAPHTREE_DISABLE_`をコメントアウトすると、`.dot`ファイルが出力されるようになります。
出力には、`graphtree.h`という、C言語でGraphvizを使用してDFSを可視化しやすくするライブラリを作って使っています。
興味があればご覧ください。
https://github.com/toduq/graphtree.h

役には立たないと思いますが、ライセンスはCC0です。ご自由にお使いください。（ソースコードが間違っていても責任は取れません。）
https://creativecommons.org/publicdomain/zero/1.0/deed.ja

以下の場合にIssueなどで教えていただけるととても喜びます。

- アルゴリズムなどが間違っていそうな時
- より良いC言語の書き方があるとき
