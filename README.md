# RealTimeSystem_Project

## Group Info : (2%)

F64096198 資訊系大三 李展瑋

F74093825 資訊系大三 王碩輔

F74109016 資訊系大三 葉惟欣
## Document your progress, thought process : (2%)
## Explain the logic of your scheduler : (2%)
## Things you learned : (2%)

### Problem we encounter :
#### **問題一 : 想要確認 output的內容**
number of Schedules: number of Schedules, same as length of input vector
number of TaskGroup: index of TaskGroup, starting from 0
這裡的number of Schedules 在json檔是對應到有幾個{} 其中大括號就是所謂的taskGroup(一定含periodic , aperiodic, sporadic task)，而每個taskGroup中各類型的task數量不一。
因此如果測資為 [{"P":[...],"A":[...],"S":[...]},
               {"P":[...],"A":[...],"S":[...]},
               {"P":[...],"A":[...],"S":[...]},}]
會印出number of Schedules = 3
一開始 number of TaskGroup = 0 ，接下來為reject or accept的內容
再來 number of TaskGroup = 1 最後 number of TaskGroup =2 。
想問助教這樣的理解是否正確?

#### **問題二 : 想確認 sporadic 與 aperiodic task 的deadline**
作業要求內容 : 
Aperiodic tasks, Sporadic tasks consist of arrival time A and Cost 
C(Worst Case Execution Time), the deadline D is equal to A+C. 
想清楚知道 這樣Aperiodic 與 Sporadic task 差別差在哪，還有只是純粹定義上
先排Sporadic task，排完後再排Aperiodic task，優先順序問題而已?

#### **問題三 : frame size。**
繼問題二，如果deadline都為A+C 那就代表所有的sporadic 與 aperiodic task 在
realease time的時候就要開始在每個time unit 中被排進去了，如果在realease 
time 不能被排進去 那就可以直接reject了。因為他們的execution time 為
deadline - release time。 
如果能夠這樣理解，那還可以繼續理解 frame size 一定要為 1 個time unit。不然一定
會有很多sporadic task 在realease 的瞬間，沒有被schedule 立刻排進去嗎?




## Correct Format : (2%)

## References : (Additional)
Read Json file : https://medium.com/ml2b/a-guide-to-json-using-c-

https://www.geeksforgeeks.org/how-to-parse-an-array-of-objects-in-cpp-using-rapidjson/

https://www.geeksforgeeks.org/rapidjson-file-read-write-in-cpp/

https://www.w3resource.com/JSON/structures.php

https://levelup.gitconnected.com/parsing-json-in-c-with-rapidjson-6d6d3e505e12

https://github.com/nlohmann/json/issues/747

https://www.geeksforgeeks.org/rapidjson-file-read-write-in-cpp/

https://blog.csdn.net/qq2399431200/article/details/108381886

https://www.geeksforgeeks.org/how-to-parse-an-array-of-objects-in-cpp-using-rapidjson/

https://stackoverflow.com/questions/70818266/

how-to-parse-json-array-from-inside-an-object-with-
https://stackoverflow.com/questions/70818266/how-to-parse-json-array-from-inside-an-object-with-rapidjson
