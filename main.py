from flask import Flask, request, jsonify, send_from_directory
import json

app = Flask(__name__)

@app.route('/data', methods=['GET'])
def get_data():
    with open('sample_data.json') as f:
        data = json.load(f)
    return jsonify(data)

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

if __name__ == '__main__':
    app.run(debug=True)