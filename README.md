# Lab 1: TCP retransmission

## Idea

If client receive data from it's receive socket buffer before retransmission, client will take the original data.

But, if not it depends on how OS's kernel controls.

What happens if retransmitted data is different from original data?

![idea](https://user-images.githubusercontent.com/17893401/131502189-3a4ed26a-4e42-45d1-adbc-4e79e98e5a4b.png)

## Result

As a result operating systems that I could test(Windows, Linux) didn't update payload to retransmitted data.

You can check out the packets I captured in testing [here](https://github.com/pol4bear/Lab-1-TCP-restransmission/raw/master/retransmit.pcapng).

