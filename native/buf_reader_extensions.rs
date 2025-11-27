use std::io::{self, BufRead, Error, ErrorKind, Read, Result, Seek, SeekFrom};

use crate::constants::BytesCursor;

pub trait BinReader {
    fn read_i32(&mut self) -> Result<i32>;

    fn read_line_trimmed(&mut self) -> Result<String>;

    fn read_n_chars(&mut self, size: usize) -> Result<String>;

    fn ensure_end_reached(&mut self) -> Result<()>;
}

impl BinReader for BytesCursor {
    fn read_i32(&mut self) -> Result<i32> {
        let mut buf = [0u8; 4];
        self.read_exact(&mut buf)?;

        Ok(i32::from_le_bytes(buf))
    }

    fn read_line_trimmed(&mut self) -> Result<String> {
        let mut line = String::new();
        self.read_line(&mut line)?;

        let len = line.len() - 1;
        line.truncate(len);

        Ok(line)
    }

    fn read_n_chars(&mut self, n: usize) -> Result<String> {
        let mut buf = vec![0u8; n];
        self.read_exact(&mut buf)?;

        String::from_utf8(buf).map_err(|e| Error::new(ErrorKind::InvalidData, e))
    }

    fn ensure_end_reached(&mut self) -> Result<()> {
        let current_pos = self.stream_position()?;
        let end_pos = self.seek(SeekFrom::End(0))?;

        if current_pos != end_pos {
            return Err(io::Error::new(
                io::ErrorKind::InvalidData,
                "Reader is not at the end",
            ));
        }

        Ok(())
    }
}
