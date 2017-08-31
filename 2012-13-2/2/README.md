# 2. Feladat - 2012/2013/2

Készítsen 2D editort, ami az egér balgomb lenyomásokat kontrolpontokként értelmezi, amelyekre egy piros Catmull-Rom (CR) spline-t és egy zöld CRI-spline-t is illeszt.

A CRI spline interpolációs, a CR-hez hasonlóan harmadfokú és a kontrolpontokban C1 folytonos, másutt C2 (ezen feltételek nem teljesítése automatikus elutasítást von maga után). A CRI sebességvektora az i. kontrolpontban a megelőző és a következő kontrolpontok közötti átmenet átlagos sebessége (p[i+1]-p[i-1])/(t[i+1]-t[i-1]) (széleken megválasztható).

A görbék a csomóértékeken közösködnek, amelyek az i. pont esetén t[i]=i+sin(i/5.0)*0.4.

A SHIFT+Z zoom-ol, azaz a kameraablak mérete felére csökken mialatt a közepe helyben marad, a SHIFT+P pedig pan-nel, azaz a kameraablak a vízszintes méretének 10%-val balra, függőleges méretének 20%-val felfelé mozdul el. A két görbén egymással szemben periódikusan egy-egy nyíl szalad, a kezdőpontja a görbe adott időponthoz (sec-ben) tartozó pontja, egyébként pedig a sebességvektor skalárszorosa (látható legyen a képen).


## Mejegyzés
NAGY Z és NAGY P!!!!!!!!!!!!


## Fordítás és futtatás

A fordításhoz szükséges a freeglut3-dev, amit a következő paranccsal telepíthetsz:

```bash
sudo apt-get install freeglut3-dev
```

Majd a program fordítása és telepítése:

``` bash
g++ grafika.cpp -o grafika -lglut -lGLU -lGL && ./grafika
```
