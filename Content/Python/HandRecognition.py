import cv2
import socket
import mediapipe as mp
import json
import time

# Initialisation des objets MediaPipe et OpenCV
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils
cap = cv2.VideoCapture(0)

# Initialisation du serveur
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

    # Conversion de l'image en RGB pour MediaPipe
    image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(image_rgb)

    # Préparation des données JSON
    hands_data = {"hands": []}
    if results.multi_hand_landmarks:
        for hand_landmarks, hand_handedness in zip(results.multi_hand_landmarks, results.multi_handedness):
            hand_type = hand_handedness.classification[0].label  # "Right" ou "Left"
            landmarks = [
                {"x": lm.x, "y": lm.y, "z": lm.z}
                for lm in hand_landmarks.landmark
            ]
            hands_data["hands"].append({"type": hand_type, "landmarks": landmarks})

    # Envoi des données JSON
    try:
        json_data = json.dumps(hands_data)
        client_socket.send(json_data.encode())
    except Exception as e:
        print("Error sending data:", e)
        break

    # Affichage des résultats
    for hand_landmarks in results.multi_hand_landmarks or []:
        mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
    cv2.imshow('Hand Detection', frame)

    # Quitter avec la touche 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
client_socket.close()
