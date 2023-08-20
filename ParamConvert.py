def fn_fs_gyr(param, mode, client, shell = None):
    # Fonction de conversion pour l'echelle de mesure du gyro
    # param : parametre recu (soit de la carte, soit de la saisie utilisateur)
    # mode : p ou r
    # client : client TCP
    # shell : affichage  ou pas sur l'IHM
    if mode == 'p':
        if param == "0":
            if shell == 1:
                print("Full scale gyro : 250 dps")
            else:
                return 250
        elif param == "1":
            if shell == 1:
                print("Full scale gyro : 500 dps")
            else:    
                return 500
        elif param == "2":
            if shell == 1:
                print("Full scale gyro : 1000 dps")
            else:
                return 1000
        elif param == "3":
            if shell == 1:
                print("Full scale gyro : 2000 dps")
            else:
                return 2000
        else:
            print("Valeur recue de fs_gyro non valide.")
    elif mode == 'r':
        if param == "250":
            client.sendall(b'250')
            return 250
        elif param == "500":
            client.sendall(b'500')
            return 500
        elif param == "1000":
            client.sendall(b'1000')
            return 1000
        elif param == "2000":
            client.sendall(b'2000')
            return 2000
        else:
            print("Valeur recue de fs_gyro non valide.")
            


def fn_fs_acc(param, mode, client, shell = None):
    # Fonction de conversion pour l'echelle de mesure de l'accelero
    # param : parametre recu (soit de la carte, soit de la saisie utilisateur)
    # mode : p ou r
    # client : client TCP
    # shell : affichage  ou pas sur l'IHM
    if mode == 'p':
        if param == "0":
            if shell == 1:
                print("Full scale accelero : 2 g")
            else:
                return 2
        elif param == "1":
            if shell == 1:
                print("Full scale accelero : 16 g")
            else:
                return 16
        elif param == "2":
            if shell == 1:
                print("Full scale accelero : 4 g")
            else:
                return 4
        elif param == "3":
            if shell == 1:
                print("Full scale accelero : 8 g")
            else:
                return 8
        else:
            print("Valeur recue de fs_acc non valide. ")
    elif mode == 'r':
        if param == "2":
            client.sendall(b'2')
            return 2
        elif param == "16":
            client.sendall(b'16')
            return 16
        elif param == "4":
            client.sendall(b'4')
            return 4
        elif param == "8":
            client.sendall(b'8')
            return 8
        else:
            print("Valeur recue de fs_acc non valide. ")

def fn_filtre_gyr(FILTRE_GYR, shell):
    # Fonction de conversion pour les filtres du gyro
    # FILTRE_GYR : liste de parametres recus des cartes
    # shell : affichage  ou pas sur l'IHM
    s_filtre = FILTRE_GYR[0]
    s_fc = FILTRE_GYR[1]
    if s_filtre == "0":
        if shell == 1:
            print("Nombre de filtres passe-bas en sortie du gyro : 1. Fréquence de coupure non réglable (1 kHz)")
        else:
            return [1,1]
    elif s_filtre == "1":
        if shell == 1:
            print("Nombre de filtres passe-bas en sortie du gyro : 2. \nL'un a une fréquence de coupure non réglable de 1 kHz, l'autre a une fréquence de coupure réglable, dont la valeur actuelle est précisée ci-dessous.")
        if s_fc == "0":
            if shell == 1:
                print("304 Hz")
            else:
                return [2, 304]
        elif s_fc == "1":
            if shell == 1:
                print("221 Hz")
            else:
                return [2, 221]
        elif s_fc == "2":
            if shell == 1:
                print("166 Hz")
            else:
                return [2, 166]
        elif s_fc == "3":
            if shell == 1:    
                print("453 Hz")
            else:
                return [2, 453]
        elif s_fc == "4":
            if shell == 1:
                print("99 Hz")
            else:
                return [2, 99]
        elif s_fc == "5":
            if shell == 1:
                print("49 Hz")
            else:
                return [2, 49]
        elif s_fc == "6":
            if shell == 1:
                print("25 Hz")
                return [2, 25]
        elif s_fc == "7":
            if shell == 1:
                print("12 Hz")
            else:
                return [2, 12]
        else:
            print("Filtres gyro : Valeur recue non valide. ")
    else:
        print("Filtres gyro : Valeur recue non valide. ")



def fn_bw_acc(param, shell):
    # Fonction de conversion pour les filtres de l'accelero
    # param : liste de parametres recus des cartes
    # shell : affichage  ou pas sur l'IHM
    s_en = param[0]
    s_fc = param[1]
    if s_en == "0":
        if shell == 1:
            print("Nombre de filtres passe-bas en sortie de l'accéléro : 1. Fréquence de coupure non réglable (830 Hz)")
        else:
            return [1,830]
    elif s_en == "1":
        if shell == 1:
            print("Nombre de filtres passe-bas en sortie de l'accéléro : 2. \nL'un a une fréquence de coupure non réglable de 830 Hz, l'autre a une fréquence de coupure réglable, dont la valeur actuelle est précisée ci-dessous.")
        if s_fc == "0":
            if shell == 1:
                print("415 Hz")
            else:
                return [2, 415]
        elif s_fc == "1":
            if shell == 1:
                print("166 Hz")
            else:
                return [2, 166]
        elif s_fc == "2":
            if shell == 1:
                print("83 Hz")
            else:
                return [2, 83]
        elif s_fc == "3":
            if shell == 1:
                print("36 Hz")
            else:
                return [2, 36]
        elif s_fc == "4":
            if shell == 1:
                print("16 Hz")
            else:
                return [2, 16]
        elif s_fc == "5":
            if shell == 1:
                print("8 Hz")
            else:
                return [2, 8]
        elif s_fc == "6":
            if shell == 1:
                print("4 Hz")
                return [2, 4]
        elif s_fc == "7":
            if shell == 1:    
                print("2 Hz")
            else:
                return [2, 2]
        else:
            print("Filtres accelero : Valeur recue non valide. ")
    else:
       print("Filtres accelero : Valeur recue non valide. ") 