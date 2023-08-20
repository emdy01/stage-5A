Ce repository contient le code développé dans le cadre d'un stage de fin d'étude. Ce stage consistait à développer d'une chaîne d'acquisition permettant de tracer l'évolution temporelle de la vitesse et de l'accélération des fauteuils roulants utilisés en athlétisme. Le système développé contient deux sous-systèmes : un sous-système électronique destiné à l'acquisition des données, et un sous-système logiciel gérant le traitement des données et l'interface homme-machine. Les deux sous-systèmes communiquent via un réseau WiFi, par le protocole TCP.

SOUS-SYSTEME ELECTRONIQUE

Il contient deux cartes Arduino et des IMU (Inertial Measurement Unit). Ces IMU contiennent entre autres des registres. Sur les cartes Arduino tourne un programme développé en C/C++ et structuré de la manière suivante :     
systeme_embarque.ino : programme principal 
IMU.cpp/IMU.h : programme de gestion des IMU 
Register.h : liste des registres utilisés et de certains codes binaires présents dans la documentation des IMU. 
DataManager.cpp/DataManager.h : programme de communication avec le sous-système logiciel

SOUS-SYSTEME LOGICIEL

Il est développé en Python et structuré de la manière suivante : 
serveur_multithreading.py : serveur multithread, programme principal 
ParamConvert.py : programme de conversion des paramètres d'acquisition 
serveur_et_echelles_de_mesure.json : paramètres réseau du serveur et échelles de mesures
