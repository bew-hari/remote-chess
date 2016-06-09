from flask import jsonify
from flask_restful import Resource, reqparse
from flask_pymongo import PyMongo

from bson.objectid import ObjectId
from bson.json_util import dumps, loads

import json
import requests
import chess
import chess.uci

from remote_chess import app
from remote_chess.config import mongo

PARTICLE_URI = app.config['PARTICLE_URI']
PHOTON_ACCESS_TOKEN = app.config['PHOTON_ACCESS_TOKEN']

class Games(Resource):
  # create new game
  def post(self):
    parser = reqparse.RequestParser()
    parser.add_argument('data')
    #parser.add_argument('board_id')
    #parser.add_argument('type', type=int) # 0 = AI, 1 = human

    args = json.loads(parser.parse_args()['data'])

    # create game with AI
    board = chess.Board()
    game = {
      '_id': str(ObjectId()),
      'board': board.fen(),
      'state': 2,
      'result': '',
      'players': [args['board_id']],
      #'type': args['type']
    }

    # save game
    id = mongo.db.games.save(game)

    # post back to white for move
    command = '~'.join([game['_id'], 'AI', '1', '0']) + '~'
    headers = {'content-type': 'application/x-www-form-urlencoded'}
    r = requests.post(
      PARTICLE_URI + args['board_id'] + '/startGame', 
      data={
        'access_token': PHOTON_ACCESS_TOKEN, 
        'args': command
      },
      headers=headers
    )

    """
    if args['type'] == 0:
      # create game with AI
      board = chess.Board()
      game = {
        '_id': str(ObjectId()),
        'board': board.fen(),
        'state': 2,
        'result': '',
        'players': [args['board_id']],
        'type': args['type']
      }

      # save game
      id = mongo.db.games.save(game)

      # TODO: post back to white for move
      command = '~'.join([game['_id'], 'AI', '1', '0']) + '~'
      r = requests.post(
        PARTICLE_URI + args['board_id'] + '/startGame', 
        {
          'access_token': PHOTON_ACCESS_TOKEN, 
          'args': 'command=' + command
        }
      )

    else:
      game = mongo.db.games.find_one({
        'type': 1,
        'state': 1,
        'players': {'$size': 1}
      })

      if game:
        # join game
        game['state'] = 2
        players = game['players']
        players.append(args['board_id'])

        # update game in database
        mongo.db.games.update(
          {'_id': game['_id']}, 
          {
            '$set': {
              'players': players,
              'state': game['state']
            },
            '$currentDate': {'lastModified': True}
          }
        )

        # TODO: post to white for move
        command = '~'.join([game['_id'], players[1], '1', '0']) + '~'
        r = requests.post(
          PARTICLE_URI + players[0] + '/startGame', 
          {
            'access_token': PHOTON_ACCESS_TOKEN, 
            'args': 'command=' + command
          }
        )
        
        # TODO: post to black to wait
        command = '~'.join([game['_id'], players[0], '0', '1']) + '~'
        r = requests.post(
          PARTICLE_URI + players[1] + '/startGame', 
          {
            'access_token': PHOTON_ACCESS_TOKEN, 
            'args': 'command=' + command
          }
        )
        
      else:
        # create game
        board = chess.Board()
        game = {
          '_id': str(ObjectId()),
          'board': board.fen(),
          'state': 1,
          'result': None,
          'players': [args['board_id']],
          'type': args['type']
        }
        
        # save game
        id = mongo.db.games.save(game)
    """

    return {
      'error': None,
      'data': game
    }, 201




class Game(Resource):
  # get specific game
  def get(self):
    parser = reqparse.RequestParser()
    parser.add_argument('game_id')
    args = parser.parse_args()

    game = mongo.db.games.find_one({'_id': args['game_id']})
    
    if game:
      res = {
        'error': None,
        'data': {
          '_id': game['_id'],
          'board': game['board'],
          'state': game['state'],
          'result': game['result'], 
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
  def post(self):
    
    print 'Got move. Parsing'
    parser = reqparse.RequestParser()
    parser.add_argument('data')
    args = json.loads(parser.parse_args()['data'])
    print 'args: '
    print args

    #parser.add_argument('board_id')
    #parser.add_argument('game_id')
    #parser.add_argument('move')
    #parser.add_argument('capture')

    #args = parser.parse_args()

    game = mongo.db.games.find_one({
      '_id': args['game_id'],
      'players': args['board_id'],
      'state': 2
    })
    print 'game: ' 
    print game
    
    if not game:
      return {
        'error': 'No game with matching id',
        'data': None
      }, 201

    player = args['board_id']
    board = chess.Board(fen=game['board'])
    print 'board :'
    print board

    uci_move = to_uci(board, args['move'], args['capture'])
    print "uci move:"
    print uci_move
    move = chess.Move.from_uci(uci_move)
    print 'Move ' 
    print move

    if board.turn != (player == game['players'][0]):
      print 'other players turn'
      command = '0'
      headers = {'content-type': 'application/x-www-form-urlencoded'}
      r = requests.post(
        PARTICLE_URI + args['board_id'] + '/error', 
        data={
          'access_token': PHOTON_ACCESS_TOKEN, 
          'args': command
        },
        headers=headers
      )
      return {
        'error': 'Other player\'s turn',
        'data': None
      }, 201

    if not move:
      print 'No move found'
      command = '1'
      headers = {'content-type': 'application/x-www-form-urlencoded'}
      r = requests.post(
        PARTICLE_URI + args['board_id'] + '/error', 
        data={
          'access_token': PHOTON_ACCESS_TOKEN, 
          'args': command
        },
        headers=headers
      )
      return {
        'error': 'Invalid move',
        'data': None
      }, 201

    if move not in board.legal_moves:
      print 'Illegal move'
      command = '2'
      headers = {'content-type': 'application/x-www-form-urlencoded'}
      r = requests.post(
        PARTICLE_URI + args['board_id'] + '/error', 
        data={
          'access_token': PHOTON_ACCESS_TOKEN, 
          'args': command
        },
        headers=headers
      )
      return {
        'error': 'Illegal move',
        'data': None
      }, 201

    board.push(move)
    result = ''
    print board

    if board.is_game_over():
      game['state'] = 3
      result = board.result()

      # player wins
      command = '0'
      headers = {'content-type': 'application/x-www-form-urlencoded'}
      r = requests.post(
        PARTICLE_URI + args['board_id'] + '/gameOver', 
        data={
          'access_token': PHOTON_ACCESS_TOKEN, 
          'args': command
        },
        headers=headers
      )
    
    else:
      # generate move for AI
      engine = chess.uci.popen_engine('./remote_chess/stockfish-7-x64-linux')
      print 'open engine'
      engine.position(board)
      print 'set board position'
      ai_move = engine.go(movetime=2000)
      print 'generate ai move'

      board.push(ai_move.bestmove)
      print ai_move.bestmove
      print board

      if board.is_game_over():
        game['state'] = 3
        result = board.result()

        # post AI move to player board
        to_move = ai_move.bestmove.uci()
        command = '~'.join([to_move, game['state'], '0']) + '~'
        headers = {'content-type': 'application/x-www-form-urlencoded'}
        r = requests.post(
          PARTICLE_URI + args['board_id'] + '/movePiece', 
          data={
            'access_token': PHOTON_ACCESS_TOKEN, 
            'args': command
          },
          headers=headers
        )

        # computer wins
        command = '1'
        headers = {'content-type': 'application/x-www-form-urlencoded'}
        r = requests.post(
          PARTICLE_URI + args['board_id'] + '/gameOver', 
          data={
            'access_token': PHOTON_ACCESS_TOKEN, 
            'args': command
          },
          headers=headers
        )

      else:
        # post AI move to player board
        to_move = ai_move.bestmove.uci()
        command = '~'.join([to_move, game['state'], '1']) + '~'
        headers = {'content-type': 'application/x-www-form-urlencoded'}
        r = requests.post(
          PARTICLE_URI + args['board_id'] + '/movePiece', 
          data={
            'access_token': PHOTON_ACCESS_TOKEN, 
            'args': command
          },
          headers=headers
        )
        print 'After posting AI move'


    # update game in database
    mongo.db.games.update(
      {'_id': args['game_id']}, 
      {
        '$set': {
          'board': board.fen(),
          'state': game['state'],
          'result': result
        },
        '$currentDate': {'lastModified': True}
      }
    )

    return {
      'error': None,
      'data': {
        '_id': game['_id'],
        'board': board.fen(),
        'state': game['state'],
        'result': result,
        'players': game['players'],
        'type': game['type']
      }
    }, 201
    


def to_uci(board, move, capture=None):
  before = ''

  for i in xrange(64):
    if board.piece_at(i):
      before += '1'
    else:
      before += '0'

  diff = '{:064b}'.format(int(before, base=2) ^ int(move, base=2))

  if capture:
    # find index of removed piece
    c_diff = '{:064b}'.format(int(before, base=2) ^ int(capture, base=2))

    count = 0
    for i in xrange(64):
      if c_diff[i] == '1' and capture[i] == '0':
        index = i
        count += 1

    # can only capture exactly one piece
    if count != 1:
      return None

    c_rank = index / 8
    c_file = index % 8
    c_dest = chr(ord('a') + c_file) + chr(ord('1') + c_rank)

    # find capturing piece
    m_diff = '{:064b}'.format(int(capture, base=2) ^ int(move, base=2))
    
    source = -1
    dest = -1
    count = 0
    for i in xrange(64):
      if m_diff[i] == '1':
        if capture[i] == '1' and move[i] == '0':
          source = i
        elif capture[i] == '0' and move[i] == '1':
          dest = i

        count += 1

    # can only move a single piece
    if count != 2:
      return None

    m_source_rank = source / 8
    m_source_file = source % 8
    m_source = chr(ord('a') + m_source_file) + chr(ord('1') + m_source_rank)

    m_dest_rank = dest / 8
    m_dest_file = dest % 8
    m_dest = chr(ord('a') + m_dest_file) + chr(ord('1') + m_dest_rank)

    if m_dest_rank == c_rank and m_dest_file == c_file:
      return m_source + c_dest
    elif (m_source_rank == c_rank and 
          abs(m_source_file - c_file) == 1 and 
          m_dest_file == c_file and 
          abs(m_dest_rank - c_rank) == 1):
      return m_source + m_dest
    else:   # invalid capture
      return None

  else:
    m_diff = '{:064b}'.format(int(before, base=2) ^ int(move, base=2))

    indices = []
    for i in xrange(64):
      if m_diff[i] == '1':
        indices.append(i)

    # regular move
    if len(indices) == 2:
      source = -1
      dest = -1
      if before[indices[0]] == '1' and move[indices[1]] == '1':
        source = indices[0]
        dest = indices[1]
      elif before[indices[1]] == '1' and move[indices[0]] == '1':
        source = indices[1]
        dest = indices[0]
      else:
        return None

      m_source_rank = source / 8
      m_source_file = source % 8
      m_source = chr(ord('a') + m_source_file) + chr(ord('1') + m_source_rank)

      m_dest_rank = dest / 8
      m_dest_file = dest % 8
      m_dest = chr(ord('a') + m_dest_file) + chr(ord('1') + m_dest_rank)

      return m_source + m_dest

    # castling
    elif len(indices) == 4:
      indices = sorted(indices)

      ranks = [i/8 for i in indices]
      files = [i%8 for i in indices]

      # not in the bottom or top ranks
      if any(r != 0 for r in ranks) or any(r != 7 for r in ranks):
        return None

      # not in castling format
      if (before[indices[0]] != '1' or
          before[indices[4]] != '1' or 
          move[indices[2]] != '1' or 
          move[indices[3]] != '1'):
        return None

      source = -1
      dest = -1
      if files == [0,2,3,4]:    # queen side castling
        source = indices[3]
        dest = indices[1]
      elif files == [4,5,6,7]:  # king side castling
        source = indices[0]
        dest = indices[2]
      else:
        return None
      
      m_source_rank = source / 8
      m_source_file = source % 8
      m_source = chr(ord('a') + m_source_file) + chr(ord('1') + m_source_rank)

      m_dest_rank = dest / 8
      m_dest_file = dest % 8
      m_dest = chr(ord('a') + m_dest_file) + chr(ord('1') + m_dest_rank)

      return m_source + m_dest

    else:
      return None
