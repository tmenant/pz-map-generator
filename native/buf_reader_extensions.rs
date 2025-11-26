use std::{
    io::{self, BufRead, BufReader, Error, ErrorKind, Read, Result, Seek, SeekFrom},
};

/// Extension trait for `BufReader` to provide additional convenience methods.
pub trait BufReaderExt {
    /// Reads exactly 4 bytes from the underlying reader and returns them as a little-endian `i32`.
    ///
    /// # Errors
    ///
    /// Returns an `Err` if reading 4 bytes fails.
    fn read_i32(&mut self) -> Result<i32>;

    /// Reads a line from the underlying reader and trims any trailing newline (`\n` or `\r\n`).
    ///
    /// # Errors
    ///
    /// Returns an `Err` if reading from the reader fails.
    fn read_line_trimmed(&mut self) -> Result<String>;

    /// Reads exactly `n` bytes from the underlying reader and attempts to convert them to a UTF-8 `String`.
    ///
    /// # Errors
    ///
    /// Returns an `Err` if reading `n` bytes fails or if the bytes are not valid UTF-8.
    fn read_n_chars(&mut self, size: usize) -> Result<String>;

    /// Compare the current cursor position to the end of the base stream, and throw an error if they are different.
    ///
    /// # Errors
    ///
    /// TODOC...
    fn ensure_end_reached(&mut self) -> Result<()>;
}

impl<R: Read + Seek> BufReaderExt for BufReader<R> {
    fn read_i32(&mut self) -> Result<i32> {
        let mut buf = [0u8; 4];
        self.read_exact(&mut buf)?;

        Ok(i32::from_le_bytes(buf))
    }

    fn read_line_trimmed(&mut self) -> Result<String> {
        let mut line = String::new();
        self.read_line(&mut line)?;

        let len = line.trim_end().len();
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
