"""Create 8x8 evolution sequences with Charmander's Poké Ball intro/timing."""
import json
from pathlib import Path
from awx import decode, encode_compact, encode
ROOT = Path(__file__).resolve().parents[1]
PALETTE = {
    '.': '#000000',
    't': '#218C73', 'T': '#63CEAD', 'h': '#A5E7BD',
    'g': '#216331', 'G': '#399C31', 'L': '#84CE42',
    'r': '#E74A63', 'w': '#FFFFFF', 'k': '#102129',
    'b': '#298CAD', 'B': '#63CEE7', 'H': '#ADEFFF',
    'p': '#F78CB5', 'P': '#FFB5D6', 'c': '#737B84', 'C': '#D6DEE7',
    'v': '#526BB5', 'V': '#8C9CDE',
    's': '#7B4229', 'S': '#BD7B42', 'y': '#DEAD5A', 'Y': '#FFE79C',
}
# Each stage has its own silhouette, face and species-specific details.
SPRITES = {
    'bulbasaur_ball': [
        ['.....G..', '....GLG.', '.t.tGLG.', 'tThTtGgt', 'twkTTTtt', 'tTtTTgt.', '.tTTTtt.', '.tt..tt.'],
        ['....rP..', '...rPPr.', '..GLrLG.', '.tgtggGt', 'tThTTTtt', 'twkTTgt.', '.tTTTtt.', '.tt..tt.'],
        ['...rPr..', '..rPYPr.', '.GrPrPrG', 'gLGgGgLG', '.tttttt.', 'twrTTwrt', 'tTTtTTTt', '.tt..tt.'],
    ],
    'squirtle_ball': [
        ['.bBBb...', 'bHBwkb..', 'bBBBBb..', '.bbssbB.', 'BbYYbSB.', '.bYYbS.B', 'bBYYbSBB', 'bbb..bb.'],
        ['.H..H...', '.vHVHv..', '.vkwVv..', '.vVVVv..', '..vYsSsH', '.vYYSsSH', '.vYYvsHH', '..vv.v.H'],
        ['C.....C.', 'cCbbbCc.', '.bHBBbs.', '.bkwBbSs', 'bbYYSsSb', 'bYYYSsSb', '.bYYbSs.', '.bb..bb.'],
    ],
}


def build():
    reference = decode((ROOT/'data/animations/charmander_ball.awx').read_bytes())
    result = {}
    for name, sprites in SPRITES.items():
        frames = list(reference['frames'][:2])
        for sprite in sprites:
            assert len(sprite) == 8 and all(len(row) == 8 for row in sprite)
            frames.append([PALETTE[pixel] for row in sprite for pixel in row])
        animation = dict(width=8, height=8, frameMs=reference['frameMs'], frames=frames)
        # Canonical RGB565 colors exactly match what the matrix will display.
        animation = decode(encode_compact(animation))
        assert encode(decode(encode_compact(animation))) == encode(animation)
        (ROOT/'examples'/f'{name}.json').write_text(json.dumps(animation, indent=2)+'\n')
        result[name] = animation
    return result

if __name__ == '__main__':
    for name, animation in build().items():
        print(name, animation['width'], animation['height'], len(animation['frames']), animation['frameMs'])
