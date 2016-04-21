from flask import jsonify
from flask_restful import Resource, reqparse
from flask_pymongo import PyMongo

from bson.objectid import ObjectId
from bson.json_util import dumps, loads

import requests
import chess

from remote_chess import mongo

class Games(Resource):
  # create new game
  def post(self):
    parser = reqparse.RequestParser()
    parser.add_argument('board_id')
    parser.add_argument('type', type=int) # 0 = AI, 1 = human

    args = parser.parse_args()

    if args['type'] == 0:
      # TODO: create game with AI
      pass

    else:
      game = mongo.db.games.find_one({
        'type': 1,
        'players': {'$size': 1}
      })

      if game:
        # join game
        players = game['players']
        players.append(args['board_id'])

        # update game in database
        mongo.db.games.update(
          {'_id': game['_id']}, 
          {
            '$set': {
              'players': players
            },
            '$currentDate': {'lastModified': True}
          }
        )
      else:
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




class Game(Resource):
  # get specific game
  def get(self, game_id):
    game = mongo.db.games.find_one({'_id': game_id})
    
    if game:
      res = {
        'error': None,
        'data': {
          '_id': game['_id'],
          'board': game['board'],
          'status': game['status'],
          'player_turn': game['player_turn'],
          'players': game['players'],
          'type': game['type']
        }
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


    if game['type'] == 0:
      # TODO: generate best move for AI
      pass
    else:
      # switch player turn
      player_turn = (game['player_turn'] + 1) % 2

      # TODO: post to other board


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

    return {
      'error': None,
      'data': {
        '_id': game['_id'],
        'board': board.fen(),
        'status': game['status'],
        'player_turn': player_turn,
        'players': game['players'],
        'type': game['type']
      }
    }, 201


