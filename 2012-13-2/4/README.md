# 4. Feladat - 2012/2013/2

Készítsen virtuális világot csempézett szobával, fókával és labdával. A padló textúrázott, diffúz/spekuláris csempe jellegű anyag. A fóka teste szabadformájú paraméteres felület (pl. Largange interpolációs felület, Bézier felület, nem klasszikus felület paraméteres formában), nem texúrázott, fekete, de spekulárisan csillogó. A fókának két szeme van, amelyeket forgathat, de a szemgolyók nem ugranak ki a szemgödörből. A strandlabda textúrázott és illuminált sárga-piros csíkos gömb.

A szobában legalább egy pont vagy irányforrás világít (lehet több is). Az 'f' billentyűvel a kamerát egy lépéssel előre, a 'b'-vel pedig hátra vihetjük.

Beadási határidő: 2013. 05. 05. 23:59


## Megoldás hiányossága

Mivel a szemgolyók mozgása feltételes mód, így az a megoldásból kimaradt.


## Fordítás és futtatás

A fordításhoz szükséges a freeglut3-dev, amit a következő paranccsal telepíthetsz:

```bash
sudo apt-get install freeglut3-dev
```

Majd a program fordítása és telepítése:

``` bash
g++ grafika.cpp -o grafika -lglut -lGLU -lGL && ./grafika
```
