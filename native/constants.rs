use std::io::Cursor;

pub const CELL_SIZE_IN_BLOCKS: usize = 32;
pub const SQR_CELL_SIZE_IN_BLOCKS: usize = CELL_SIZE_IN_BLOCKS * CELL_SIZE_IN_BLOCKS;
pub const LINE_END: [u8; 1] = [0xA; 1];

pub type BytesCursor = Cursor<Vec<u8>>;
