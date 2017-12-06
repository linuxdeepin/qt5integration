<a name=""></a>
##  0.2.8.3 (2017-12-06)


#### Bug Fixes

* **style:**  visable bottom lines on QMenu when hover item ([c8eb56b7](https://github.com/linuxdeepin/qt5integration/commit/c8eb56b7cadb8f7a3d95207bf6ff7a7048a89706))



<a name=""></a>
##  0.2.8.2 (2017-12-06)


#### Features

* **theme:**  set window flags for native dialog ([3ec67bf0](https://github.com/linuxdeepin/qt5integration/commit/3ec67bf01fead41dbac5a4b096622f7ca50d685c))

#### Bug Fixes

*   is dxcb platform if the "_d_isDxcb" property is true of QCoreApplication ([714b5f78](https://github.com/linuxdeepin/qt5integration/commit/714b5f78b98f7e63757db15ab84a8eb653c36b31))
* **style:**
  *  visable left/top lines on draw rubber band control ([9c06e770](https://github.com/linuxdeepin/qt5integration/commit/9c06e7709b99958e6ebb98714fbbfa2a61d7a0a5))
  *  bottom/right visible lines on draw rubber band rect for QListView ([1177381a](https://github.com/linuxdeepin/qt5integration/commit/1177381ae5914188ca4236e993475367071d66bd))
  *  slider icons ([dae8caa6](https://github.com/linuxdeepin/qt5integration/commit/dae8caa6ec73b95b3781ebe6ecd6da0b359a189f))



<a name=""></a>
##  0.2.8.1 (2017-11-02)


#### Bug Fixes

*   remove libqt5dxcb-dev for "Conflicts" and "Provides" ([4d38dbca](https://github.com/linuxdeepin/qt5integration/commit/4d38dbcac71375f96469cfdda2b4bad0124c7b16))



<a name=""></a>
##  0.2.8 (2017-11-02)


#### Features

*   keep the window border is 1px for hiDPI ([da8040c4](https://github.com/linuxdeepin/qt5integration/commit/da8040c42df0cbde97e21693f472a288ef46a9d8))
* **theme plugin:**  read settings from config file ([aacc2995](https://github.com/linuxdeepin/qt5integration/commit/aacc299512f4006ff174c5700d471d344d7155d0))
* **theme style:**  auto update the widgets font when theme config file changed ([d478074e](https://github.com/linuxdeepin/qt5integration/commit/d478074e73d8e22e2d70080ad1430c565261d9af))

#### Bug Fixes

*   add build-depend: libegl1-mesa-dev ([2d265267](https://github.com/linuxdeepin/qt5integration/commit/2d265267ad1f6f113a74c931a67fe938b85a5af0))
*   build failed(add build-depend: libmtdev-dev) ([babdc356](https://github.com/linuxdeepin/qt5integration/commit/babdc3561ab22d85ab83e4b6e385a4daa1387516))
*   set cursor is invaild ([a2e235be](https://github.com/linuxdeepin/qt5integration/commit/a2e235bed7873d35030be380e8cd73acc0192e89))
* **dxcb:**
  *  crash when screen changed ([7e1627c2](https://github.com/linuxdeepin/qt5integration/commit/7e1627c20fd92d7b4c1d7a69c55b4de5869cf6b6))
  *  the window border size is wrong ([83d6ac50](https://github.com/linuxdeepin/qt5integration/commit/83d6ac500b7e070f5c97ff220da16736229fd060))
  *  draw shadow area ([284cae9d](https://github.com/linuxdeepin/qt5integration/commit/284cae9dd90e9b9d7af92480e3a0e217a7c5f478))
  *  the DFrameWindow shadow area ([6e9c8274](https://github.com/linuxdeepin/qt5integration/commit/6e9c8274ef871fe3fa48a60a0861a96b6880dbf0))
* **style:**
  *  update all widgets on font changed ([0e28f244](https://github.com/linuxdeepin/qt5integration/commit/0e28f244217f57d454424cc7dfba488ee6be93e3))
  *  update all widgets on font changed ([d4a65deb](https://github.com/linuxdeepin/qt5integration/commit/d4a65deb9e21e18390447c8b26eb429843333be4))
* **theme:**
  *  "DFileSystemWatcherPrivate::addPaths: inotify_add_watch failed" ([a0322a68](https://github.com/linuxdeepin/qt5integration/commit/a0322a68447b7d918af49fbba23ebe1d5f53da25))
  *  set ini codec to "utf8" for QSettings ([3a4f7d76](https://github.com/linuxdeepin/qt5integration/commit/3a4f7d7651f6c433ef9735426d936289ac04d6c9))



