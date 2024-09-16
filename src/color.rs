use serenity::all::Colour;

// these colors are taken straight up from gh/cephxdev/rubiko by the way
const COLORS: [Colour; 28] = [
    Colour(0x1abc9c), // aqua
    Colour(0x57f287), // green
    Colour(0x3498db), // blue
    Colour(0xfee75c), // yellow
    Colour(0x9b59b6), // purple
    Colour(0xe91e63), // luminous vivid pink
    Colour(0xeb459e), // fuchsia
    Colour(0xf1c40f), // gold
    Colour(0xe67e22), // orange
    Colour(0xed4245), // red
    Colour(0x95a5a6), // grey
    Colour(0x34495e), // navy
    Colour(0x11806a), // dark aqua
    Colour(0x1f8b4c), // dark green
    Colour(0x206694), // dark blue
    Colour(0x71368a), // dark purple
    Colour(0xad1457), // dark vivid pink
    Colour(0xc27c0e), // dark gold
    Colour(0xa84300), // dark orange
    Colour(0x992d22), // dark red
    Colour(0x979c9f), // dark grey
    Colour(0x7f8c8d), // darker grey
    Colour(0xbcc0c0), // light grey
    Colour(0x2c3e50), // dark navy
    Colour(0x5865f2), // blurple
    Colour(0x99aab5), // greyple
    Colour(0x2c2f33), // dark but not black
    Colour(0x23272a) // not quite black
];

///
/// Get a random color
///
/// # Returns
///
/// A random color
///
pub fn rand() -> Colour {
    let index = rand::random::<usize>() % COLORS.len();
    COLORS[index]
}
