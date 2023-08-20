import socket, sys, select
from signal import *
import ParamConvert
import json
import matplotlib.pyplot as plt
from time import *
from datetime import date
import subprocess
from threading import *

# Chargement des parametres reseau du serveur et des echelles de mesures par defaut
with open("serveur_et_echelles_de_mesure.json", "r") as res:
    dict_global_param = json.load(res)
    s_ADRESSE = dict_global_param['s_ADRESSE'] # Adresse du serveur
    i_PORT = dict_global_param["i_PORT"] # Port d'ecoute du serveur
    i_fs_gyr = dict_global_param['i_fs_gyr'] # Echelle de mesure par defaut du gyro en dps
    i_fs_acc = dict_global_param["i_fs_acc"] # Echelle de mesure par defaut de l'accelero en g
    i_fs_imu = dict_global_param['i_fs_imu'] # Pleine echelle IMU en bits


# Coefficients de conversion des donnees recuperees par la carte en donnees interpretables par l'utilisateur
coef_gyr = i_fs_gyr/i_fs_imu
coef_acc = i_fs_acc*9.81/i_fs_imu

Param = []  # Liste remplie avec les parametres de la carte

s_DATE = str(date.today().day) + "-" + str(date.today().month)   

# Flags de controle des threads
f_startDebug = Event()
f_startMode = Event()
f_startSetting = Event()
f_startDebug.clear()
f_startMode.clear()
f_startSetting.clear()

l_end_of_mode = [] # Liste remplie par les threads lorsqu'ils ont fini d'executer un mode


def fn_initialise_mode_a():
    # initialisation des structures necessaires au stockage des donnees provenant des cartes

    # Thread 1
    a_datasets1 = [] # Stockage des json
    Gyr11=[[], [], []] # IMU1 - Gyro 
    Gyr12=[[], [], []] # IMU2 - Gyro
    Gyr13=[[], [], []] # IMU3 - Gyro
    Gyr14=[[], [], []] # IMU4 - Gyro
    Acc11=[[], [], []] # IMU1 - Accelero
    Acc12=[[], [], []] # IMU2 - Accelero
    Acc13=[[], [], []] # IMU3 - Accelero
    Acc14=[[], [], []] # IMU4 - Accelero
    a_data_gyr1 = [Gyr11, Gyr12, Gyr13, Gyr14]
    a_data_acc1 = [Acc11, Acc12, Acc13, Acc14]
    a_T1 = [] # Stockage des valeurs de temps


    # Thread 2
    a_datasets2 = [] # Stockage des json
    Gyr21=[[], [], []] # IMU1 - Gyro 
    Gyr22=[[], [], []] # IMU2 - Gyro
    Gyr23=[[], [], []] # IMU3 - Gyro
    Gyr24=[[], [], []] # IMU4 - Gyro
    Acc21=[[], [], []] # IMU1 - Accelero
    Acc22=[[], [], []] # IMU2 - Accelero
    Acc23=[[], [], []] # IMU3 - Accelero
    Acc24=[[], [], []] # IMU4 - Accelero
    a_data_gyr2 = [Gyr21, Gyr22, Gyr23, Gyr24]
    a_data_acc2 = [Acc21, Acc22, Acc23, Acc24]
    a_T2 = [] # Stockage des valeurs de temps

    return a_T1, a_T2, a_datasets1, a_datasets2, a_data_gyr1, a_data_acc1, a_data_gyr2, a_data_acc2

def fn_write_json():
    # Ecriture des fichiers json correspondant aux deux threads
    with open(filename +"_1.json", "w") as res:
        for json_file in a_datasets1:     
            json.dump(json_file, res) # Ecriture dans le fichier des json créés par le logiciel
            res.write("\n")
          

    with open(filename +"_2.json", "w") as res:
        for json_file in a_datasets2:     
            json.dump(json_file, res) # Ecriture dans le fichier des json créés par le logiciel
            res.write("\n")
         

def fn_affichage_graphique():
    # Affichage graphique des mesures envoyees par les cartes (mode a)

    #Premiere figure
    plt.figure("Boitier")
    ax = plt.subplot(421)
    plt.plot(a_T1, a_data_gyr1[0][0], 'r--')
    plt.plot(a_T1, a_data_gyr1[0][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_gyr1[0][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU1 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))
    

    ax = plt.subplot(422)
    plt.plot(a_T1, a_data_acc1[0][0], 'r--')
    plt.plot(a_T1, a_data_acc1[0][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_acc1[0][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU1 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))
    
    

    ax = plt.subplot(423)
    plt.plot(a_T1, a_data_gyr1[1][0], 'r--')
    plt.plot(a_T1, a_data_gyr1[1][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_gyr1[1][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU2 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))


    ax = plt.subplot(424)
    plt.plot(a_T1, a_data_acc1[1][0], 'r--')
    plt.plot(a_T1, a_data_acc1[1][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_acc1[1][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU2 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))

    

    ax = plt.subplot(425)
    plt.plot(a_T1, a_data_gyr1[2][0], 'r--')
    plt.plot(a_T1, a_data_gyr1[2][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_gyr1[2][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU3 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))

    

    ax = plt.subplot(426)
    plt.plot(a_T1, a_data_acc1[2][0], 'r--')
    plt.plot(a_T1, a_data_acc1[2][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_acc1[2][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU3 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))   
    

    ax = plt.subplot(427)
    plt.plot(a_T1, a_data_gyr1[3][0], 'r--')
    plt.plot(a_T1, a_data_gyr1[3][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_gyr1[3][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU4 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))
    

    ax = plt.subplot(428)
    plt.plot(a_T1, a_data_acc1[3][0], 'r--')
    plt.plot(a_T1, a_data_acc1[3][1], 'bs', markersize = 0.75)
    plt.plot(a_T1, a_data_acc1[3][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU4 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))
    

    plt.tight_layout()

    #Deuxieme figure
    plt.figure("Carte factice")
    ax = plt.subplot(421)
    plt.plot(a_T2, a_data_gyr2[0][0], 'r--')
    plt.plot(a_T2, a_data_gyr2[0][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_gyr2[0][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU1 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))


    ax = plt.subplot(422)
    plt.plot(a_T2, a_data_acc2[0][0], 'r--')
    plt.plot(a_T2, a_data_acc2[0][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_acc2[0][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU1 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))


    ax = plt.subplot(423)
    plt.plot(a_T2, a_data_gyr2[1][0], 'r--')
    plt.plot(a_T2, a_data_gyr2[1][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_gyr2[1][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU2 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))
    

    ax = plt.subplot(424)
    plt.plot(a_T2, a_data_acc2[1][0], 'r--')
    plt.plot(a_T2, a_data_acc2[1][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_acc2[1][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU2 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))

    

    ax = plt.subplot(425)
    plt.plot(a_T2, a_data_gyr2[2][0], 'r--')
    plt.plot(a_T2, a_data_gyr2[2][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_gyr2[2][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU3 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))

    

    ax = plt.subplot(426)
    plt.plot(a_T2, a_data_acc2[2][0], 'r--')
    plt.plot(a_T2, a_data_acc2[2][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_acc2[2][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU3 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))   

    ax = plt.subplot(427)
    plt.plot(a_T2, a_data_gyr2[3][0], 'r--')
    plt.plot(a_T2, a_data_gyr2[3][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_gyr2[3][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU4 - Gyro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("dps")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))


    ax = plt.subplot(428)
    plt.plot(a_T2, a_data_acc2[3][0], 'r--')
    plt.plot(a_T2, a_data_acc2[3][1], 'bs', markersize = 0.75)
    plt.plot(a_T2, a_data_acc2[3][2], 'g^', markersize = 0.75)
    plt.grid(True)
    plt.title("IMU4 - Accéléro", fontsize = 10)
    plt.xlabel("temps (s)")
    plt.ylabel("m/s²")
    ax.legend("xyz", loc = 'center right', bbox_to_anchor = (1.1,0.5))


    plt.tight_layout()

    print("Done.")
    plt.show()
    
def fn_affichage_param(): 
    # Affichage des parametres envoyes par les cartes (modes p et r)
    print("Fe = ",Param[0], "Hz")
    ParamConvert.fn_fs_acc(Param[1], 'p', None, 1)
    ParamConvert.fn_fs_gyr(Param[2], 'p', None, 1)
    print("\n")
    l_filtre_gyr = [Param[3],Param[4]]
    ParamConvert.fn_filtre_gyr(l_filtre_gyr,1)
    print("\n")
    l_bw_accel = [Param[5],Param[6]]
    ParamConvert.fn_bw_acc(l_bw_accel,1)

def fn_saisie_nouv_param():
    # interface utilisateur pour la saisie des parametres souhaites (mode r).
    # fonction appelee depuis le programme principal
    global i_Fe, i_fs_acc, i_fs_gyr, LPF_acc, s_lpf_gyr, l_bw_acc, s_bw_gyr # ces variables doivent etre globales 
    #pour pouvoir etre reprises dans les threads
    i_Fe = input("Fe : ")
    print("Choisissez l'échelle de mesure de l'accéléromètre parmi les valeurs suivantes (en g) : 2, 4, 8, 16.")
    i_fs_acc = input()
    while (i_fs_acc not in ['2','4', '8', '16']): 
        print("Valeur non valide. Veuillez choisir un élément de la liste.")
        i_fs_acc = input("Full scale accelero : ")
    print("Choisissez l'échelle de mesure du gyromètre parmi les valeurs suivantes (en dps) : 250, 500, 1000, 2000.")
    i_fs_gyr = input()
    while (i_fs_gyr not in ['250','500', '1000', '2000']): 
        print("Valeur non valide. Veuillez choisir un élément de la liste.")
        i_fs_gyr = input()
    print("Voulez-vous placer 1 ou 2 filtres passe-bas en sortie du gyro ?")
    s_lpf_gyr = input()
    while (s_lpf_gyr not in ['1','2']):
        print("Valeur non valide. Veuillez saisir 1 ou 2.")
        s_lpf_gyr = input() 
    if s_lpf_gyr == '1':
        s_bw_gyr = None
    else:
        print("Vous avez la possibilité de choisir la fréquence de coupure de l'un des filtres. \nChoisissez-la parmi les valeurs suivantes (en Hz) : 12, 25, 49, 99, 166, 221, 304, 453.")
        s_bw_gyr = input()
        while (s_bw_gyr not in ['12', '25', '49', '99', '166', '221', '304', '453']): 
            print("Valeur non valide. Veuillez choisir un élément de la liste.")
            s_bw_gyr = input()
    print("Voulez-vous placer 1 ou 2 filtres passe-bas en sortie de l'accelero ?")
    LPF_acc = input()
    while (LPF_acc not in ['1','2']):
        print("Valeur non valide. Veuillez saisir 1 ou 2.")
        LPF_acc = input()
    if LPF_acc == '1':
        l_bw_acc = None
    else:
        print("Vous avez la possibilité de choisir la fréquence de coupure de l'un des filtres. \nChoisissez-la parmi les valeurs suivantes (en Hz) : 2, 4, 8, 16, 36, 83, 166, 415.")
        l_bw_acc = input()
        while (l_bw_acc not in ['2', '4', '8', '16', '36', '83', '166', '415']): 
            print("Valeur non valide. Veuillez choisir un élément de la liste.")
            l_bw_acc = input()

def fn_close(): 
    # arret du serveur
    print('Arret du serveur.')
    serveur.close()
    sys.exit()

def fn_fct_thread(client, i_DataID):
    # fonction-cible des threads
    global Param, i_fs_gyr, i_fs_acc # ces variables doivent etre globales pour pouvoir etre reprises dans le programme principal
    f_startDebug.wait() # attente de l'ordre d'activation (ou pas) du mode debug
    # puis envoi de cet ordre aux cartes
    if b_debug == 0:
        client.sendall(b'0')
    else:
        client.sendall(b'1')
    while True:
        f_startMode.wait() # attente du choix du mode par l'utilisateur
        if s_mode == 'a' :  # si le mode acquisition est activé
            # envoi du mode et de la duree d'acquisition souhaitee
            client.sendall(b'a')                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
            client.sendall(bytes(param, 'utf-8'))

            # reception et traitement des donnees recues de la carte
            s_buffer_size = client.recv(3).decode()    
            while (s_buffer_size !="EOS"):
                s_buffer_size = int(s_buffer_size)
                s_data = client.recv(s_buffer_size).decode()
                if s_data == "EOS":
                    break
                if s_data[-1] != "E":
                    EndTest = client.recv(1).decode()
                    while str(EndTest) != "E":
                        s_data += str(EndTest)
                        EndTest = client.recv(1).decode()
                else:
                    s_data = s_data[:-1]
        
                a_all4imu = s_data.split(";")
                for i in range(4): # pour chaque IMU
                    dict_bloc  = {}
                    TMP= a_all4imu[i].split(",")
                    # reformatage des donnees si necessaires
                    if len(TMP) == 6:
                        if i_DataID == 1:
                            TMP.append(str(a_data_acc1[3][2][-1]))  
                        if i_DataID == 2:
                            TMP.append(str(a_data_acc2[3][2][-1])) 
                    if len(TMP) > 7:
                        TMP = TMP[-7:]
                        TMP[0] = TMP[0][-1] 
                    if i == 0:
                        if  i_DataID == 1: 
                            #a_T1.append(float(TMP[0])/1000)
                            a_T1.append(1)
                        if  i_DataID == 2:
                           #a_T2.append(float(TMP[0])/1000) 
                           a_T2.append(1) 
                    # Creation du json et mise a jour des structures de stockage                        
                    TMP_GYR = [(float(TMP[1]))*coef_gyr, (float(TMP[2]))*coef_gyr, (float(TMP[3]))*coef_gyr]
                    TMP_ACC = [(float(TMP[4]))*coef_acc, (float(TMP[5]))*coef_acc, (float(TMP[6]))*coef_acc]
                    dict_bloc["Tps" + str(i+1)]=TMP[0]
                    dict_bloc["Gyr " +str(i+1) + "_X"]=TMP_GYR[0]
                    dict_bloc["Gyr" + str(i+1) + "_Y"]=TMP_GYR[1]
                    dict_bloc["Gyr" + str(i+1) + "_Z"]=TMP_GYR[2]
                    dict_bloc["Acc" + str(i+1) + "_X"]=TMP_ACC[0]
                    dict_bloc["Acc" + str(i+1) + "_Y"]=TMP_ACC[1]
                    dict_bloc["Acc" + str(i+1) + "_Z"]=TMP_ACC[2]
                    
                    if i_DataID==1:
                        a_datasets1.append(dict_bloc)
                        for j in range(3):
                            a_data_gyr1[i][j].append(TMP_GYR[j])
                            a_data_acc1[i][j].append(TMP_ACC[j])
                    if i_DataID==2:
                        a_datasets2.append(dict_bloc)
                        for j in range(3): 
                            a_data_gyr2[i][j].append(TMP_GYR[j])
                            a_data_acc2[i][j].append(TMP_ACC[j])

                
                
                s_buffer_size = client.recv(3).decode()

            # Creation du json recapitulant les parametres d'acquisition utilises
            dict_bloc = {}
            tmp =client.recv(20).decode()
            Param = tmp.split(",")
            dict_bloc["Fe"] = int(Param[0])
            dict_bloc["Full scale accelero"] = ParamConvert.fn_fs_acc(Param[1], 'p',0)
            dict_bloc["Full scale gyro"] = ParamConvert.fn_fs_gyr(Param[2], 'p',0)
            l_filtre_gyr = [Param[3],Param[4]]
            tmp = ParamConvert.fn_filtre_gyr(l_filtre_gyr,0)
            if tmp[0] == 1:
                dict_bloc["Nb filtres pour gyro"] = 1
            if tmp[0] == 2:
                dict_bloc["Nb filtres pour gyro"] = 2 
                dict_bloc["Fqce de coupure reglable filtre gyro (Hz)"] = tmp[1] 
            l_bw_accel = [Param[5],Param[6],1]
            tmp = ParamConvert.fn_bw_acc(l_bw_accel,0)
            if tmp[0] == 1:
                dict_bloc["Nb filtres pour accelero"] = 1
            if tmp[0] == 2:
                dict_bloc["Nb filtres pour accelero"] = 2 
                dict_bloc["Fqce de coupure reglable filtre accelero (Hz)"] = tmp[1] 
            
            if i_DataID==1:
                a_datasets1.append(dict_bloc)
            if i_DataID==2:
                a_datasets2.append(dict_bloc)


            l_end_of_mode.append(True) # signalement au programme principal de la fin d'execution du mode
            f_startMode.clear()
        if s_mode == 'p': # si le mode parametres est activé
            # envoi du mode
            client.sendall(b'p') 
            # reception des parametres de la carte
            tmp =client.recv(20).decode()
            Param = tmp.split(",")
            l_end_of_mode.append(True)  # signalement au programme principal de la fin d'execution du mode
            f_startMode.clear()
        if s_mode == 's': # si le mode stop est activé
            # envoi du mode
            client.sendall(b's')
            l_end_of_mode.append(True)  # signalement au programme principal de la fin d'execution du mode
            f_startMode.clear()
            return   
        if s_mode == 'r': # si le mode reglages est activé
            # envoi du mode
            client.sendall(b'r')
            # reception des parametres de la carte
            tmp =client.recv(20).decode()
            Param = tmp.split(",")
            l_end_of_mode.append(True)  # signalement au programme principal de la reception complete des parametres de la carte
            f_startSetting.wait() #attente de la saisie complete des nouveaux parametres d'acquisitions souhaites
            # Envoi des nouveaux parametres d'acquisitions
            client.sendall(i_Fe.encode())
            sleep(2)
            i_fs_acc = ParamConvert.fn_fs_acc(i_fs_acc,'r',client)
            sleep(2)
            i_fs_gyr = ParamConvert.fn_fs_gyr(i_fs_gyr,'r',client)
            sleep(2)
            client.sendall(s_lpf_gyr.encode())
            sleep(2)
            if s_bw_gyr != None:
                client.sendall(s_bw_gyr.encode())
                
            sleep(2)
            client.sendall(LPF_acc.encode())
            sleep(2)
            if l_bw_acc != None:
                client.sendall(l_bw_acc.encode() )
            l_end_of_mode.append(True)  # signalement au programme principal de la fin d'execution du mode
            f_startMode.clear() 



# Demarrage du serveur et initialisation des threads
serveur = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serveur.bind((s_ADRESSE, i_PORT))   
serveur.listen(1)
print("Listening on", s_ADRESSE, ", port", i_PORT)
print("Veuillez allumer les boîtiers.")

client, s_adresseClient = serveur.accept()
print('Connexion de ', s_adresseClient)
thread1 = Thread(target=fn_fct_thread,args=(client, 1))
client, s_adresseClient = serveur.accept()
print('Connexion de ', s_adresseClient)
thread2 = Thread(target=fn_fct_thread,args=(client, 2))
thread1.start()
thread2.start()

# Activation (ou pas) du mode debug
print("Voulez-vous activer le debug ? 1 : oui ; 0 : non")
s_debug = input()
while (s_debug not in ["0","1"]):
    print("Tapez 0 ou 1.")
    s_debug=  input()
f_startDebug.set()
b_debug = (s_debug == "1")

while (True):
        # choix du mode
        print("a : lancer une acquisition. p : afficher les paramètre d'acquisition. r : régler les paramètres d'acquisition. s : arrêter le système.")
        s_mode = input()
        while ((s_mode != 'a') and (s_mode != 'r') and (s_mode != 's') and (s_mode != 'p')):
            print("Tapez a, p, r ou s.")
            s_mode =  input()
        if s_mode == 'a': # si le mode acquisition est activé

            # initialisation
            filename = input(f"Name of datafile (default {s_DATE}):") or s_DATE
            param = input("Duration of the experiment (in s):")
            a_T1, a_T2, a_datasets1, a_datasets2, a_data_gyr1, a_data_acc1, a_data_gyr2, a_data_acc2 = fn_initialise_mode_a()

            # lancement des threads
            f_startMode.set()

            # attente de la fin des threads
            while l_end_of_mode != [True, True]:
                pass

            l_end_of_mode = []

            fn_write_json() #ecriture du fichier de controle
            fn_affichage_graphique() #affichage graphique des mesures recues
        
        if s_mode == 'p': # si le mode parametres est activé
            # lancement des threads
            f_startMode.set()
            # attente de la fin des threads
            while l_end_of_mode != [True, True]:
                pass
            
            l_end_of_mode = []
        
            fn_affichage_param() # affichage des parametres d'acquisition

        if s_mode == 's': # si le mode stop est activé
            # lancement des threads
            f_startMode.set()
            # attente de la fin des threads
            while l_end_of_mode != [True, True]:
                pass 
            # arret du logiciel
            fn_close()

        if s_mode == 'r': # si le mode reglages est activé
            # lancement des threads
            f_startMode.set()
            # attente de la fin des threads
            while l_end_of_mode != [True, True]:
                pass
            
            l_end_of_mode = []
            
            print("Paramètres actuels :")
            fn_affichage_param()

            print("----------------------------")
            print("Nouvelles valeurs souhaitées :")
            fn_saisie_nouv_param()
            # lancement des threads
            f_startSetting.set()
            # attente de la fin des threads
            while l_end_of_mode != [True, True]:
                pass
            print(i_fs_gyr)
            print(i_fs_acc)
            l_end_of_mode = []
            print("Réglages des capteurs terminés.")
            # Mise a jour des coefficients de conversion des donnees recuperees par la carte en donnees interpretables par l'utilisateur
            coef_gyr = i_fs_gyr/i_fs_imu
            coef_acc = i_fs_acc*9.81/i_fs_imu

            