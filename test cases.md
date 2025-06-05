Here are some test results you can try to see if the code is running for you.
You will see that the code fails in instances where the same token is being used more than once. Like in test 7 where t1 is used twice and in test 5. 

You will need to enter the rules and then the string afterwards.
For example:
t1 a|b , t2 a*.a , t3 (a|b)*.c*.c #
"a aac bbc aabc"

--1---------------------------------------------

t1 a|b , t2 a*.a , t3 (a|b)*.c*.c #
"a aac bbc aabc"

t1 , "a"
t3 , "aac"
t3 , "bbc"
t3 , "aabc"

--2---------------------------------------------

t1 a|b , t2 a.a* , t3 (a|b)*.(c|d)#
"aaabbcaaaa"

t3 , "aaabbc"
t2 , "aaaa"

--3---------------------------------------------

t1 a|b , t2 a*.a , t3 a*, t4 b , t5 (a|b)* #
"a aac bbc aabc"

EPSILON IS NOT A TOKEN t3 t5


--4---------------------------------------------

t1 a|b , t2 a*.a , t3 (a|b)*.c #
"a aa bbc aad aa"

t1 , "a"
t2 , "aa"
t3 , "bbc"
t2 , "aa"
ERROR

--5---------------------------------------------

t1 a , t2 a.a , t3 a.a.a , t4 b#
"aaaba"

t3 , "aaa"
t4 , "b"
t1 , "a"

--6---------------------------------------------

t1 a.b.(c.d)* , t2 b.cd* , t3 c.d*, t4 d #
"abcdcdd"

t1 , "abcdcd"
t4 , "d"

--7---------------------------------------------

t1 a.b.r.a , t2 a.c.a , t3 d.a.b , t4 c.a.d. #
"abracadabra"

t1 , "abra"
t4 , "cad"
t1 , "abra"

--8---------------------------------------------

t1 a|b|c|d , t2 (a|b)*.(b|c|d) , t3 a.a.(h|i|j)*.(a|b)*
"baaabababadabcaaja"

t2 , "baaabababad"
t2 , "abc"
t3 , "aaja"
