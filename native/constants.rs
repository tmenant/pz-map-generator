use std::io::Cursor;

pub const MIN_LAYER: i32 = -32;
pub const MAX_LAYER: i32 = 32;

pub const BLOCK_SIZE_IN_SQUARE: usize = 8;
pub const CELL_SIZE_IN_BLOCKS: usize = 32;
pub const SQR_CELL_SIZE_IN_BLOCKS: usize = CELL_SIZE_IN_BLOCKS * CELL_SIZE_IN_BLOCKS;

pub const LINE_END: [u8; 1] = [0xA; 1];

pub type BytesBuf = Vec<u8>;
pub type BytesCursor = Cursor<BytesBuf>;
