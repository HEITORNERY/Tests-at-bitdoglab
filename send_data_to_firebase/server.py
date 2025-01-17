from flask import Flask, request
from google.cloud import firestore
import os

app = Flask(__name__)

# Configurar o caminho para o arquivo de credenciais
os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = "path/to/your/credentials.json"

# Inicializar o cliente do Firestore
db = firestore.Client()

@app.route('/update', methods=['GET'])
def update():
    temp = request.args.get('temp')
    if temp:
        print(f"Received temperature: {temp} °C")
        # Enviar dados para o Firestore
        doc_ref = db.collection('temperatures').document()
        doc_ref.set({
            'temperature': float(temp),
            'timestamp': firestore.SERVER_TIMESTAMP
        })
        return f"Temperature {temp} °C received and stored in Firestore", 200
    else:
        return "No temperature data received", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
