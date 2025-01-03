import cv2
import mediapipe as mp
import json
import os
import time
import shutil

# Initialisation des objets MediaPipe et OpenCV
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils
cap = cv2.VideoCapture(0)

# Chemins des fichiers JSON
temp_file = "hand_data.tmp"
final_file = "hand_data.json"
json_temp_file_path = os.path.join(os.path.dirname(__file__), temp_file)
json_file_path = os.path.join(os.path.dirname(__file__), final_file)

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

    # Écriture des données JSON dans un fichier temporaire
    try:
        with open(json_temp_file_path, "w") as json_file:
            json.dump(hands_data, json_file)

        # Supprimer le fichier final s'il existe
        if os.path.exists(json_file_path):
            os.remove(json_file_path)

        # Renommage du fichier temporaire vers le fichier final
        shutil.move(json_temp_file_path, json_file_path)
    except Exception as e:
        print("Error handling JSON file:", e)
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
