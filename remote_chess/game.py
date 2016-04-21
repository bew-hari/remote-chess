from flask import jsonify
from flask_restful import Resource, reqparse
from flask_pymongo import PyMongo

from bson.objectid import ObjectId
from bson.json_util import dumps

import requests
import chess

from remote_chess import mongo

class Games(Resource):
  # create new game
  def post(self):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('type') # 0 = AI, 1 = human

    args = parser.parse_args()

    #if args['type'] == 0:
      # create game
    board = chess.Board()
    game = {
      '_id': str(ObjectId()),
      'board': board.fen(),
      'status': 0,
      'player_turn': 0,
      'players': [args['board_id']],
      'type': args['type']
    }
    
    # save game
    id = mongo.db.games.save(game)


    return {
      'error': None,
      'data': game
    }, 201
    #else:
      # TODO: post to other board




class Game(Resource):
  # get specific game
  def get(self, game_id):
    game = mongo.db.games.find_one({'_id': game_id})
    
    if game:
      res = {
        'error': None,
        'data': game
      }
    else:
      res = {
        'error': 'No game with matching _id',
        'data': None
      }

    return res, 201
  
  # modify specific game
  def post(self, game_id):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('move')

    args = parser.parse_args()

    game = mongo.db.games.find_one({'_id': game_id})

    if not game:
      return {
        'error': 'No game with matching id',
        'data': None
      }, 201

    board = chess.Board(fen=game['board'])
    move = chess.Move.from_uci(args['move'])

    if game['players'][game['player_turn']] != args['board_id']:
      return {
        'error': 'Other player\'s turn',
        'data': None
      }, 201

    if move not in board.legal_moves:
      return {
        'error': 'Illegal move',
        'data': None
      }, 201

    board.push(move)

    # update game in database
    mongo.db.games.update(
      {'_id': game_id}, 
      {
        '$set': {
          'board': board.fen(),
          'player_turn': player_turn
        },
        '$currentDate': {'lastModified': True}
      }
    )

    if game['type'] == 0:
      # TODO: generate best move for AI
      pass
    else:
      # switch player turn
      player_turn = (board['player_turn'] + 1) % 2

      # TODO: post to other board

    return {
      'error': None,
      'data': game
    }, 201


