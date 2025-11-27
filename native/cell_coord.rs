#[derive(Clone, Copy, PartialEq, Eq, Hash)]
pub struct CellCoord(u64);

impl CellCoord {
    pub fn new(chunk_idx: u16, x: u16, y: u16, z: i8) -> Self {
        let z = (z + 32) as u64 & 0x7F;
        let y = y as u64 & 0x1FF;
        let x = x as u64 & 0x1FF;
        let chunk_idx = chunk_idx as u64 & 0x3FF;

        CellCoord((chunk_idx << 25) | (x << 16) | (y << 7) | z)
    }

    pub fn chunk_idx(&self) -> u16 {
        ((self.0 >> 25) & 0x3FF) as u16
    }

    pub fn x(&self) -> u16 {
        ((self.0 >> 16) & 0x1FF) as u16
    }

    pub fn y(&self) -> u16 {
        ((self.0 >> 7) & 0x1FF) as u16
    }

    pub fn z(&self) -> i8 {
        ((self.0 & 0x7F) as i8) - 32
    }

    pub fn unpack(&self) -> (u16, u16, u16, i8) {
        (self.chunk_idx(), self.x(), self.y(), self.z())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_cellcoord_bounds() {
        let coords = [
            (0, 0, 0, -32),        // z min
            (1023, 300, 300, 32),  // z max and x/y max, chunk_idx max
            (512, 150, 150, -5),   // neutral value
            (0, 0, 0, 32),         // z max, min x/y/chunk
            (1023, 0, 0, -32),     // z min, max chunk_idx
            (0, 300, 300, 0),      // x/y max, z neutre
            (1023, 300, 300, -32), // combo max/min
        ];

        for &(chunk, x, y, z) in &coords {
            let coord = CellCoord::new(chunk, x, y, z);

            assert_eq!(coord.chunk_idx(), chunk);
            assert_eq!(coord.x(), x);
            assert_eq!(coord.y(), y);
            assert_eq!(coord.z(), z);

            let (c_chunk, c_x, c_y, c_z) = coord.unpack();
            assert_eq!(chunk, c_chunk);
            assert_eq!(x, c_x);
            assert_eq!(y, c_y);
            assert_eq!(z, c_z);
        }
    }
}
