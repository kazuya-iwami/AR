C++の統合開発環境はClionを使ってます

便利なのでここを触る人はググってインストールしてみて下さい


プロジェクトを開く際はCMakeListをOverrideせずに既存のものを使う方を選択して下さい

サーバーの流れ

起動
↓
プレイヤーのアプリがid 1,2,3,4の順に接続してくる
↓
ここからは各プレイヤーがメッセージを投げるとrecv_message(string msg, int id)が呼ばれる
あるプレイヤーにメッセージを投げるときはvoid send_message(std::string msg, int id)を呼ぶ（id省略で全体送信）



実行例
hostname is kazuya-2.local
プレイヤー:0が接続しました
プレイヤー:1が接続しました
プレイヤー:2が接続しました
プレイヤー:3が接続しました
接続完了
 プレイヤー：0 HELLO WORLD
 プレイヤー：1 HELLO!!
プレイヤー:0が切断しました
Process finished with exit code


石見
