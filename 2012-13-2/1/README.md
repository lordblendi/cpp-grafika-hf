# 1. Feladat - 2012/2013/2

Készítsen breakout játékot, fejet hajtva a játékfejlesztés úttörői előtt. A 2D squash szobának három oldala van, mérete 40x40 méter. A falak 1 m vastagok. A fehér oldalfalak jól oda vannak betonozva, a piros hátsó fal téglái azonban kiüthetők (a téglák száma megválasztható, de legalább 5 és de a tégla területe, és így a tömege a labdáénál nagyobb kell hogy legyen, hogy vissza tudja lökni a labdát).

A játékosnak egy 4 m magas, 1 m vastag téglalap alakú ütője van, amit az 'i' (föl), 'j' (balra), 'k' (jobbra) és 'm' (le) billentyűkkel mozgathat.

A szobában egy 1 m sugarú sárga, köralakú labda pattog, vízszintes kezdősebessége 10 m/s, függőleges kezdősebessége 2 m/s, a falakkal rugalmasan ütközik (energia és impulzus megmarad).

 Az zöld ütő gyorsítja a labdát, egy ütéstől a mozgási energia 20%-kal nő. Az ütő sarkai le vannak gyalulva, azaz az ütközési normális a sarkon 45 fokos, ami lehetőséget ad arra, hogy az ütő sarkával is odasózzunk a labdának. A hátsó fal téglái az ütközés hatására a fizikai törvényeknek megfelelően kirepülnek. Súrlódás nincs. A szobát felülnézetből látjuk, a 2D kamera a szobán túl még egy 5 méteres sávot is befog. A használható OpenGL függvények: glBegin, glColor, glVertex2f, glEnd, glClearColor, glClear, glViewport, glMatrixMode, glLoadIdentity, gluOrtho2D Extra élvezet profiknak: ne csak az impulzus, hanem a perdület is maradjon meg az ütközésnél.

Beadási határidő: 2013. 03. 10. 23:59


## Fordítás és futtatás

A fordításhoz szükséges a freeglut3-dev, amit a következő paranccsal telepíthetsz:

```bash
sudo apt-get install freeglut3-dev
```

Majd a program fordítása és telepítése:

``` bash
g++ grafika.cpp -o grafika -lglut -lGLU -lGL && ./grafika
```
