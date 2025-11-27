use std::io::{self, Write};

use crate::constants::LINE_END;

pub trait BinWriter {
    fn write_i32(&mut self, value: i32) -> io::Result<()>;
    fn write_string(&mut self, value: &str) -> io::Result<()>;
    fn write_line(&mut self, value: &str) -> io::Result<()>;
}

impl BinWriter for Vec<u8> {
    fn write_i32(&mut self, value: i32) -> io::Result<()> {
        self.write_all(&value.to_le_bytes())
    }

    fn write_string(&mut self, value: &str) -> io::Result<()> {
        self.write_all(&value.as_bytes())
    }

    fn write_line(&mut self, value: &str) -> io::Result<()> {
        self.write_all(&value.as_bytes())?;
        self.write(&LINE_END)?;

        Ok(())
    }
}
