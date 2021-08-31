# Lab 1: TCP retransmission

## Idea

If client receive data from it's receive socket buffer before retransmission, client will take the original data.

But, if not it depends on how OS's kernel controls.

What happens if retransmitted data is different from original data?

![idea](https://user-images.githubusercontent.com/17893401/131502189-3a4ed26a-4e42-45d1-adbc-4e79e98e5a4b.png)

