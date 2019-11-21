# HTTP-API: alloc_info 

```http://localhost:8888/api/http/alloc_info```

## Purpose

Request a JSON encoded sub-list of allocation descriptors from the Memory Manager’s alloc_info list.

## Query String Parameters
| Parameter|Default|Description                       |
|-------------|----|----------------------------------|
| ```start``` | 0  | starting index of the sub-list.  |
| ```count``` | 20 | number of allocation descriptors.|

### EXAMPLE:

```http://localhost:8888/api/http/alloc_info?start=20&count=2```

## Query Response

Returns a JSON object containing four name-value pairs:

### JSON Response Object

| Name              | Value Description                       |
|-------------------|-----------------------------------------|
| ```alloc_total``` | Total number allocations in the Memory Manager’s alloc_info list. |
| ```chunk_size```  | Number of allocation description objects in ```alloc_list```. |
| ```chunk_start``` | The Memory Manager alloc_info index of the first ```alloc_list``` element below|
| ```alloc_list```  | Array of JSON Allocation Description objects (described below). |


### JSON Allocation Description Object

| Name          | Value Description                                                               |
|---------------|---------------------------------------------------------------------------------|
| ```name```    | Name of the allocation. May be ```Null```                                       |
| ```start```   | Starting address of the allocation.                                             |
| ```end```     | Ending address of the allocation.                                               |
| ```num```     |                                                                                 |
| ```size```    | Size of the allocation in bytes.                                                |
| ```type```    | Type descriptor of the allocation.                                              |
| ```stcl```    | Storage class of the allocation. Either ```TRICK_EXTERN``` or ```TRICK_LOCAL```.|
| ```language```| Language. Either : ```Language_C``` or ```Language_CPP```.                      |
| ```index```   | Array dimension sizes of the allocation (if it represents an array).            |


## Example:

In ```SIM_cannon_numeric``` (one of Trick's example sims) the following query resulted in the following JSON.

#### Query

```http://localhost:8888/api/http/alloc_info?start=20&count=2```

#### Response

```json
{ "alloc_total":43,
  "chunk_size":2,
  "chunk_start":20,
  "alloc_list":[
                 { "name":"dyn",
                   "start":"0x101aa9900",
                   "end":"0x101aa9b27",
                   "num":"1",
                   "size":"552",
                   "type":"CannonSimObject",
                   "stcl":"TRICK_EXTERN",
                   "language":"Language_CPP",
                   "index": []
                 }
                 ,
                 { "name":"web",
                   "start":"0x101aa9610",
                   "end":"0x101aa98ff",
                   "num":"1",
                   "size":"752",
                   "type":"WebServerSimObject",
                   "stcl":"TRICK_EXTERN",
                   "language":"Language_CPP",
                   "index": []
                 }
               ]
}

```