from flask import Flask
from flask_restful import Resource, Api

app = Flask(__name__)
api = Api(app)

class test(Resource):
    def post(self):
        return {'status': 'success'}

api.add_resource(test, '/test')

if __name__ == '__main__':
    app.run(debug=True)
