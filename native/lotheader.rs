use std::{
    fs::{self},
    io::{Cursor, Read, Result, Seek, Write},
};

use crate::constants::{BytesCursor, SQR_CELL_SIZE_IN_BLOCKS};
use crate::{buf_reader_extensions::BinReader, buf_writer_extensions::BinWriter};

type ZSpawnArray = [u8; SQR_CELL_SIZE_IN_BLOCKS];

pub struct Rectangle {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub heigth: i32,
}

pub struct RoomObject {
    pub room_type: i32,
    pub x: i32,
    pub y: i32,
}

pub struct Building {
    pub id: i32,
    pub room_ids: Vec<i32>,
}

pub struct Room {
    pub id: i32,
    pub name: String,
    pub layer: i32,
    pub area: i32,
    pub rectangles: Vec<Rectangle>,
    pub room_objects: Vec<RoomObject>,
}

pub struct Lotheader {
    pub magic: String,
    pub version: i32,
    pub tile_names: Vec<String>,
    pub width: i32,
    pub height: i32,
    pub min_layer: i32,
    pub max_layer: i32,
    pub rooms: Vec<Room>,
    pub buildings: Vec<Building>,
    pub spawns: ZSpawnArray,
}

// Lotheader reader
impl Lotheader {
    pub fn from_file(path: &str) -> Result<Lotheader> {
        let buf = fs::read(path)?;
        let mut cursor = Cursor::new(buf);

        Lotheader::from_bytes(&mut cursor)
    }

    pub fn from_bytes(cursor: &mut BytesCursor) -> Result<Lotheader> {
        cursor.seek(std::io::SeekFrom::Start(0))?;

        let lotheader = Lotheader {
            magic: cursor.read_n_chars(4)?,
            version: cursor.read_i32()?,
            tile_names: Lotheader::read_tilenames(cursor)?,
            width: cursor.read_i32()?,
            height: cursor.read_i32()?,
            min_layer: cursor.read_i32()?,
            max_layer: cursor.read_i32()? + 1,
            rooms: Lotheader::read_rooms(cursor)?,
            buildings: Lotheader::read_buildings(cursor)?,
            spawns: Lotheader::read_spawns(cursor)?,
        };

        cursor.ensure_end_reached()?;

        Ok(lotheader)
    }

    fn read_tilenames(cursor: &mut BytesCursor) -> Result<Vec<String>> {
        let tiles_count = cursor.read_i32()?;
        let mut tilenames = Vec::with_capacity(tiles_count as usize);

        for _ in 0..tiles_count {
            let tilename = cursor.read_line_trimmed()?;
            tilenames.push(tilename);
        }

        Ok(tilenames)
    }

    fn read_rooms(cursor: &mut BytesCursor) -> Result<Vec<Room>> {
        let rooms_count = cursor.read_i32()?;
        let mut rooms = Vec::with_capacity(rooms_count as usize);

        for id in 0..rooms_count {
            let name = cursor.read_line_trimmed()?;
            let layer = cursor.read_i32()?;
            let rectangles = Lotheader::read_rectangles(cursor)?;
            let room_objects = Lotheader::read_room_objects(cursor)?;
            let mut area = 0;

            for rect in &rectangles {
                area += rect.width * rect.heigth;
            }

            rooms.push(Room {
                id,
                name,
                layer,
                area,
                rectangles,
                room_objects,
            });
        }

        Ok(rooms)
    }

    fn read_rectangles(cursor: &mut BytesCursor) -> Result<Vec<Rectangle>> {
        let capacity = cursor.read_i32()? as usize;
        let mut rectangles = Vec::with_capacity(capacity);

        let mut buf = vec![0u8; capacity * 16]; // 16 bytes per rectangle
        cursor.read_exact(&mut buf)?;

        for chunk in buf.chunks_exact(16) {
            let x = i32::from_le_bytes(chunk[0..4].try_into().unwrap());
            let y = i32::from_le_bytes(chunk[4..8].try_into().unwrap());
            let width = i32::from_le_bytes(chunk[8..12].try_into().unwrap());
            let heigth = i32::from_le_bytes(chunk[12..16].try_into().unwrap());

            rectangles.push(Rectangle {
                x,
                y,
                width,
                heigth,
            });
        }

        Ok(rectangles)
    }

    fn read_room_objects(cursor: &mut BytesCursor) -> Result<Vec<RoomObject>> {
        let capacity = cursor.read_i32()? as usize;
        let mut room_objects = Vec::with_capacity(capacity);

        let mut buf = vec![0u8; capacity * 12]; // 12 bytes per RoomObject
        cursor.read_exact(&mut buf)?;

        for chunk in buf.chunks_exact(12) {
            let room_type = i32::from_le_bytes(chunk[0..4].try_into().unwrap());
            let x = i32::from_le_bytes(chunk[4..8].try_into().unwrap());
            let y = i32::from_le_bytes(chunk[8..12].try_into().unwrap());

            room_objects.push(RoomObject { room_type, x, y });
        }

        Ok(room_objects)
    }

    fn read_buildings(cursor: &mut BytesCursor) -> Result<Vec<Building>> {
        let building_count = cursor.read_i32()?;
        let mut buildings = Vec::with_capacity(building_count as usize);

        for i in 0..building_count {
            let rooms_count = cursor.read_i32()? as usize;

            let mut buf = vec![0u8; rooms_count * 4]; // 4 bytes per Room
            cursor.read_exact(&mut buf)?;

            let room_ids = buf
                .chunks_exact(4)
                .map(|chunk| i32::from_le_bytes(chunk.try_into().expect("Chunk size should be 4")))
                .collect();

            buildings.push(Building { id: i, room_ids });
        }

        Ok(buildings)
    }

    fn read_spawns(cursor: &mut BytesCursor) -> Result<ZSpawnArray> {
        let mut buf = [0u8; SQR_CELL_SIZE_IN_BLOCKS];
        cursor.read_exact(&mut buf)?;

        Ok(buf)
    }
}

// Lotheader writer
impl Lotheader {
    pub fn to_bytes(&self) -> Result<Vec<u8>> {
        let mut buffer = Vec::new();

        buffer.write_string(&self.magic)?;
        buffer.write_i32(self.version)?;

        self.write_tiles_def(&mut buffer)?;

        buffer.write_i32(self.width)?;
        buffer.write_i32(self.height)?;

        buffer.write_i32(self.min_layer)?;
        buffer.write_i32(self.max_layer - 1)?;

        self.write_rooms(&mut buffer)?;
        self.write_buildings(&mut buffer)?;
        self.write_spawns(&mut buffer)?;

        Ok(buffer)
    }

    fn write_tiles_def(&self, buffer: &mut Vec<u8>) -> Result<()> {
        buffer.write_i32(self.tile_names.len() as i32)?;

        for tilename in &self.tile_names {
            buffer.write_line(&tilename)?;
        }

        Ok(())
    }

    fn write_rooms(&self, buffer: &mut Vec<u8>) -> Result<()> {
        buffer.write_i32(self.rooms.len() as i32)?;

        for room in &self.rooms {
            buffer.write_line(&room.name)?;
            buffer.write_i32(room.layer)?;
            buffer.write_i32(room.rectangles.len() as i32)?;

            for rect in &room.rectangles {
                buffer.write_i32(rect.x)?;
                buffer.write_i32(rect.y)?;
                buffer.write_i32(rect.width)?;
                buffer.write_i32(rect.heigth)?;
            }

            buffer.write_i32(room.room_objects.len() as i32)?;

            for room_object in &room.room_objects {
                buffer.write_i32(room_object.room_type)?;
                buffer.write_i32(room_object.x)?;
                buffer.write_i32(room_object.y)?;
            }
        }

        Ok(())
    }

    fn write_buildings(&self, buffer: &mut Vec<u8>) -> Result<()> {
        buffer.write_i32(self.buildings.len() as i32)?;

        for building in &self.buildings {
            buffer.write_i32(building.room_ids.len() as i32)?;

            for room_id in &building.room_ids {
                buffer.write_i32(*room_id)?;
            }
        }

        Ok(())
    }

    fn write_spawns(&self, buffer: &mut Vec<u8>) -> Result<()> {
        buffer.write_all(&self.spawns)
    }
}
