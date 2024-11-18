import cv2
import socket
import mediapipe as mp
import os

# Initialisation des objets MediaPipe pour la détection des mains et OpenCV pour la capture vidéo
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils

# Ouvrir la caméra
cap = cv2.VideoCapture(0)
print("OpenCV version:", cv2.__version__)

print("Mediapipe module found at:", os.path.dirname(mp.__file__))

with open("C:/Users/Rom1/Desktop/python_script_status.txt", "w") as f:
    f.write("Python script started successfully.\n")
    


def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("localhost", 9999))
    server_socket.listen(1)
    print("Server is listening on port 9999...")
    client_socket, addr = server_socket.accept()
    print("Connection from:", addr)
    return client_socket

if __name__ == "__main__":
    client_socket = start_server()

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Conversion de l'image en format RGB pour MediaPipe
    image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(image_rgb)

    # Vérifier si des mains sont détectées
    if results.multi_hand_landmarks:
        for hand_landmarks, hand_handedness in zip(results.multi_hand_landmarks, results.multi_handedness):
            # Dessiner les points et connexions de la main
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS) 

            # Identifier la main droite ou gauche
            hand_type = hand_handedness.classification[0].label  # "Right" ou "Left"
            
            # Envoi du label de la main (droite ou gauche)
            data = f"{hand_type}:"
            
            # Iterer sur chaque landmark et envoyer ses informations
            for landmark in hand_landmarks.landmark:
                print(hand_landmarks.landmark)
                position = (landmark.x, landmark.y, landmark.z)  # Position du landmark
                
                # Ajouter le label du landmark et ses coordonnées au message
                data += f"X={position[0]}, Y={position[1]}, Y={position[2]}/"

            data += f"\n"
            
        # Envoi des données sur le serveur
        try:
            client_socket.send(data.encode())
            print(data)
        except Exception as e:
            print("Error sending data:", e)
            break

    # Afficher le résultat
    cv2.imshow('Hand Detection', frame)

    # Quitter avec la touche 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libération des ressources
cap.release()
cv2.destroyAllWindows()
client_socket.close()