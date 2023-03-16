# Transport-Layer-Sender-Visualization
visualize the sender‘s info: Congesion Control State Machine, inflight, throught...

need install libmysqlclient-dev

mkdir build
g++ main.cpp sock_data.cpp tool.cpp db.cpp -o ./build/state_machine -lmysqlclient