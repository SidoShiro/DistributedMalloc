# DistributedMalloc

Yet another malloc working on distributed systems. (Big Malloc)

# Todo

* command line
* graph loading
  * from .dot (network and capacities of each node)
  * map generation
* leader election

## Important structs/classes

* **Node**
* **Message**
  * id_s (source)
  * id_t (target)
  * id_o (final target for operation)
  * size
  * address
  * enum *Op*
    * OK
    * WRITE
    * READ
    * SNAP
    * LEADER
    * WHOISLEADER
    * (REVIVE)
    * (DIE)
    * (TEST)
* **Block**
  * Linked list of:
    * address
    * size
    * id (the machine containing this address)
    
## Command Line

| **Command**                       | **Desc.**                                               |
|-----------------------------------|---------------------------------------------------------|
| h                                 | display all available commands with their description   |
| m `size`                          | return `address` to cmd user of the required allocation |
| f `address`                       | free address, Warning if already free                   |
| w `address` `datasize` `data`     | write at the address the data of size datasize          |
| r `address` `datasize`            | read datasize bytes at address                          |
| d `address`                       | dump in as text all data of the block stored in address |
| d `address` `file`                | dump address data in file                               |
| snap                              | give user a snap of the network (print)                 |
| snap `file`                       | same as snap but stored in file                         |
| dnet                              | write a .dot file of the network                        |
| kill `id`                         | kill node refered by `id`                               |
| revive `id`                       | revive `id`                                             |
