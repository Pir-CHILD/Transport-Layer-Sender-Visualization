# Transport-Layer-Sender-Visualization
visualize the sender‘s Congesion Control State(Open, Disorder, CWR, Recovery, Loss).

need install `libmysqlclient-dev` and [SystemTap](https://sourceware.org/systemtap/).

```shell
> mkdir -p ./build; cd ./build
> cmake ../src/
> make
> ./state_machine --sIP dest_ip
```
You can see more info by type `./state_machine --help`.
```shell
> ./state_machine --help
Usage: ./state_machine [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  --sIP TEXT                  server IP
  -i,--info TEXT              test info