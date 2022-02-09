import socket
import json
import tkinter as tk
from functools import partial
import threading
import time

HOST = "127.0.0.1"
PORT = 1100


soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
soc.connect((HOST, PORT))
Format = "UTF-8"
stop_thread = False


def rejestracja(json):
    soc.send(bytes("Rejestracja", Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["uzytkownik"], Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["haslo"], Format))
    odp=soc.recv(1024)
    if "TAK" in odp.decode(Format):
        return "Rejestracja poprawna!"
    else:
        print("Błąd Rejestracji!")



def logowanie(json):
    soc.send(bytes("Logowanie", Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["uzytkownik"], Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["haslo"], Format))
    odp=soc.recv(1024)
    if "TAK" in odp.decode(Format):
        soc.send(bytes("Reload", Format))
        odp = soc.recv(1024)
        odp = odp.decode(Format)
        odp = odp.replace(',]', ']')
        return "Logowanie poprawne!"
    else:
        print("Błąd przy logowaniu!")


def dod_znaj(json):
    soc.send(bytes("Dod_znaj", Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["uzytkownik"], Format))
    odp=soc.recv(1024)
    if 'NIE' in odp.decode(Format):
        print("Błąd przy dodawaniu!")
    else:
        soc.send(bytes("Reload", Format))
        odp=soc.recv(1024)
        odp = odp.decode(Format)
        odp = odp.replace(',]', ']')
        print(odp)
        return "Dodano znajomego!"


def wysylanie(json):
    soc.send(bytes("Wysylanie", Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["uzytkownik"], Format))
    odp=soc.recv(1024)
    soc.send(bytes(str(len(json["wiadomosc"])), Format))
    odp=soc.recv(1024)
    soc.send(bytes(json["wiadomosc"], Format))
    odp=soc.recv(1024)
    if 'NIE' in odp.decode(Format):
        print("Błąd przy wysyłaniu!")
    else:
        soc.send(bytes("Reload", Format))
        odp=soc.recv(1024)
        odp = odp.decode(Format)
        odp = odp.replace(',]', ']')
        print("Wysłano wiadomość!")
        return odp



def refresh():
    soc.send(bytes("Reload", Format))
    odp = soc.recv(1024)
    odp = odp.decode('UTF-8')
    odp = odp.replace(',]', ']')
    odp = str(odp).strip("'<>() ").replace('\'', '\"')
    return odp


class GUI:
    # constructor method
    def __init__(self):
        def log():
            tkWindow = tk.Tk()
            tkWindow.geometry('400x150')
            tkWindow.title('Chat app')

            # chat window which is currently hidden
            def validateLogin(username, password):
                user = username.get()
                pas = password.get()
                print("username entered :", user)
                print("password entered :", pas)
                if user =="" or pas=="":
                    tkWindow.destroy()
                    log()

                if logowanie({'uzytkownik': user, 'haslo': pas}) == "Logowanie poprawne!":
                    print("Im in")
                    tkWindow.destroy()
                    menu()
                else:
                    tkWindow.destroy()
                    log()

            # username label and text entry box
            usernameLabel = tk.Label(tkWindow, text="User Name").grid(row=1, column=1)
            username = tk.StringVar()
            usernameEntry = tk.Entry(tkWindow, textvariable=username).grid(row=1, column=2)

            # password label and password entry box
            passwordLabel = tk.Label(tkWindow, text="Password").grid(row=2, column=1)
            password = tk.StringVar()
            passwordEntry = tk.Entry(tkWindow, textvariable=password, show='*').grid(row=2, column=2)

            validateLogin = partial(validateLogin, username, password)

            def goReg():
                tkWindow.destroy()
                reg()

            # login button
            loginButton = tk.Button(tkWindow, text="LOGIN", command=validateLogin).grid(row=5, column=2)
            ex = tk.Button(tkWindow, text="REGISTER", command=goReg).grid(row=6, column=2)
            tkWindow.mainloop()

        def reg():
            print("REJESTRACJA")
            regWindow = tk.Tk()
            regWindow.geometry('400x150')
            regWindow.title('Chat app')

            # chat window which is currently hidden
            def validateReg(username, password):
                user = username.get()
                pas = password.get()
                print("username entered :", user)
                print("password entered :", pas)
                if user=="" or pas=="":
                    regWindow.destroy()
                    reg()
                else:

                    if rejestracja({'uzytkownik': user, 'haslo': pas})=="Rejestracja poprawna!":
                        print("Rejestracja poprawna!")
                        regWindow.destroy()
                        log()


            # username label and text entry box
            usernameLabel = tk.Label(regWindow, text="User Name").grid(row=1, column=1)
            username = tk.StringVar()
            usernameEntry = tk.Entry(regWindow, textvariable=username).grid(row=1, column=2)

            # password label and password entry box
            passwordLabel = tk.Label(regWindow, text="Password").grid(row=2, column=1)
            password = tk.StringVar()
            passwordEntry = tk.Entry(regWindow, textvariable=password, show='*').grid(row=2, column=2)

            validateReg = partial(validateReg, username, password)

            # login button
            loginButton = tk.Button(regWindow, text="REGISTER", command=validateReg).grid(row=5, column=2)
            regWindow.mainloop()

        def menu():
            root = tk.Tk()
            root.geometry("200x200")

            def goChatting():
                root.destroy()
                chatting(clicked.get())

            def goAddFriend():
                root.destroy()
                addFr()

            options=[]
            data = refresh()
            dane = json.loads(data)
            for i in range(len(dane.get("znajomi"))):
                dane1 = ((dane.get("znajomi")[i]))
                for key, value in dane1.items():
                    if key == "uzytkownik":
                        options.append(value)
            if options:
                label = tk.Label(root, text="Choose your chat").grid(row=1, column=2)
                clicked = tk.StringVar()
                clicked.set(options[0])
                drop = tk.OptionMenu(root, clicked, *options).grid(row=2, column=2)
                button = tk.Button(root, text="Open Chat", command=goChatting).grid(row=3, column=2)
            addFriend = tk.Button(root, text="Add new friend", command=goAddFriend).grid(row=4, column=2)
            root.mainloop()

        def addFr():
            tkWindow = tk.Tk()
            tkWindow.geometry('400x150')
            tkWindow.title('Chat app')

            # chat window which is currently hidden
            def validateFriend(username):
                user = username.get()
                print("username entered :", user)
                if user=="":
                    tkWindow.destroy()
                    addFr()
                else:
                    dod_znaj({'uzytkownik': user})
                    print("Added")
                    tkWindow.destroy()
                    menu()

            # username label and text entry box
            usernameLabel = tk.Label(tkWindow, text="User Name").grid(row=1, column=1)
            username = tk.StringVar()
            usernameEntry = tk.Entry(tkWindow, textvariable=username).grid(row=1, column=2)

            validateFriend = partial(validateFriend, username)
            loginButton = tk.Button(tkWindow, text="ADD", command=validateFriend).grid(row=3, column=2)
            tkWindow.mainloop()

        def chatting(friendName):


            def sendMsg():  # send messages
                msg = txtMsg.get('0.0', tk.END)
                msg = msg[:-1]
                if msg!="":
                    txtMsgList.delete('0.0', tk.END)
                    wysylanie({"uzytkownik": friendName, "wiadomosc": msg})
                    data = refresh()
                    dane = json.loads(data)
                    for i in range(len(dane.get("znajomi"))):
                        dane1 = ((dane.get("znajomi")[i]))
                        Flaga = False
                        for key, value in dane1.items():
                            if key == "uzytkownik" and value == friendName:
                                Flaga = True
                            if key == "wiadomosci" and Flaga == True:
                                Flaga = False
                                for i in range(len(value)):
                                    for k, v in value[i].items():
                                        wiadomosc = ""
                                        if k == "autor":
                                            wiadomosc = wiadomosc + v + " : "
                                        if k == "wyslany_tekst":
                                            wiadomosc = wiadomosc + v + "\n"

                                        txtMsgList.insert(tk.END, wiadomosc)
                    txtMsgList.see(tk.END)
                    txtMsg.delete('0.0', tk.END)



            def cancelMsg():  # Cancel message
                stop_thread = True
                app.destroy()
                menu()

            def sendMsgEvent(event):  # Send message event
                if event.keysym == 'Up':
                    sendMsg()


            def refMsg():
                print("xd")
                while True:
                    if stop_thread:
                        break
                    time.sleep(1)
                    txtMsgList.delete('0.0', tk.END)
                    data = refresh()
                    dane = json.loads(data)
                    for i in range(len(dane.get("znajomi"))):
                        dane1 = ((dane.get("znajomi")[i]))
                        Flaga = False
                        for key, value in dane1.items():
                            if key == "uzytkownik" and value == friendName:
                                Flaga = True
                            if key == "wiadomosci" and Flaga == True:
                                Flaga = False
                                for i in range(len(value)):
                                    for k, v in value[i].items():
                                        wiadomosc = ""
                                        if k == "autor":
                                            wiadomosc = wiadomosc + v + " : "
                                        if k == "wyslany_tekst":
                                            wiadomosc = wiadomosc + v + "\n"

                                        txtMsgList.insert(tk.END, wiadomosc)
                    txtMsgList.see(tk.END)


            # Create window
            app = tk.Tk()
            app.title('Chat with ' + friendName)

            # Create a frame container
            frmLT = tk.Frame(width=500, height=320, bg='grey')
            frmLC = tk.Frame(width=500, height=100, bg='grey')
            frmLB = tk.Frame(width=500, height=30)
            frmRT = tk.Frame(width=200, height=500)

            # Create control
            txtMsgList = tk.Text(frmLT)
            txtMsgList.tag_config('greencolor', foreground='#008C00')  # Create tag
            txtMsg = tk.Text(frmLC)
            txtMsg.bind("<KeyPress-Up>", sendMsgEvent)
            btnSend = tk.Button(frmLB, text='send', width=8, command=sendMsg)
            btnCancel = tk.Button(frmLB, text='cancel', width=8, command=cancelMsg)
            #btnRef = tk.Button(frmLB, text='refresh', width=8, command=refMsg)

            # Window layout
            frmLT.grid(row=0, column=0, columnspan=2, padx=1, pady=3)
            frmLC.grid(row=1, column=0, columnspan=2, padx=1, pady=3)
            frmLB.grid(row=2, column=0, columnspan=2)
            frmRT.grid(row=0, column=2, rowspan=3, padx=2, pady=3)

            # Fixed size
            frmLT.grid_propagate(0)
            frmLC.grid_propagate(0)
            frmLB.grid_propagate(0)
            frmRT.grid_propagate(0)

            btnSend.grid(row=2, column=0)
            btnCancel.grid(row=2, column=1)
            #btnRef.grid(row=2, column=2)
            txtMsgList.place(height=320)
            txtMsg.place(height=100)

            stop_thread = False
            x = threading.Thread(target=refMsg, args=())
            x.start()
            # Main event loop
            app.mainloop()

        log()


if __name__ == '__main__':
    GUI()
