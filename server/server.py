from flask import Flask, request

app = Flask(__name__)

@app.route('/update', methods=['GET'])
def update():
    temp = request.args.get('temp')
    if temp:
        print(f"Received temperature: {temp} °C")
        return f"Temperature {temp} °C received", 200
    else:
        return "No temperature data received", 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)