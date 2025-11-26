use std::{
    fs::File,
    io::{BufReader, Read, Result, Seek},
};

use crate::buf_reader_extensions::BufReaderExt;
use crate::constants::SQR_CELL_SIZE_IN_BLOCKS;

#[allow(dead_code)]
pub struct Rectangle {
    pub x: i32,
    pub y: i32,
    pub width: i32,
    pub heigth: i32,
}

#[allow(dead_code)]
pub struct RoomObject {
    pub room_type: i32,
    pub x: i32,
    pub y: i32,
}

#[allow(dead_code)]
pub struct Building {
    pub id: i32,
    pub room_ids: Vec<i32>,
}

#[allow(dead_code)]
pub struct Room {
    pub id: i32,
    pub name: String,
    pub layer: i32,
    pub area: i32,
    pub rectangles: Vec<Rectangle>,
    pub room_objects: Vec<RoomObject>,
}

#[allow(dead_code)]
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
    pub spawns: [u8; SQR_CELL_SIZE_IN_BLOCKS],
}

impl Lotheader {
    pub fn read(path: &str) -> Result<Lotheader> {
        let file = File::open(path)?;

        let mut reader = BufReader::new(file);

        let lotheader = Lotheader {
            magic: reader.read_n_chars(4)?,
            version: reader.read_i32()?,
            tile_names: Lotheader::read_tilenames(&mut reader)?,
            width: reader.read_i32()?,
            height: reader.read_i32()?,
            min_layer: reader.read_i32()?,
            max_layer: reader.read_i32()?,
            rooms: Lotheader::read_rooms(&mut reader)?,
            buildings: Lotheader::read_buildings(&mut reader)?,
            spawns: Lotheader::read_spawns(&mut reader)?,
        };

        reader.ensure_end_reached()?;

        Ok(lotheader)
    }

    fn read_tilenames<R: Read + Seek>(reader: &mut BufReader<R>) -> Result<Vec<String>> {
        let tiles_count = reader.read_i32()?;
        let mut tilenames = Vec::with_capacity(tiles_count as usize);

        for _ in 0..tiles_count {
            let tilename = reader.read_line_trimmed()?;
            tilenames.push(tilename);
        }

        Ok(tilenames)
    }

    fn read_rooms<R: Read + Seek>(reader: &mut BufReader<R>) -> Result<Vec<Room>> {
        let rooms_count = reader.read_i32()?;
        let mut rooms = Vec::with_capacity(rooms_count as usize);

        for id in 0..rooms_count {
            let name = reader.read_line_trimmed()?;
            let layer = reader.read_i32()?;
            let rectangles = Lotheader::read_rectangles(reader)?;
            let room_objects = Lotheader::read_room_objects(reader)?;
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

    fn read_rectangles<R: Read + Seek>(reader: &mut BufReader<R>) -> Result<Vec<Rectangle>> {
        let capacity = reader.read_i32()? as usize;
        let mut rectangles = Vec::with_capacity(capacity);

        let mut buf = vec![0u8; capacity * 16]; // 16 bytes per rectangle
        reader.read_exact(&mut buf)?;

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

    fn read_room_objects<R: Read + Seek>(reader: &mut BufReader<R>) -> Result<Vec<RoomObject>> {
        let capacity = reader.read_i32()? as usize;
        let mut room_objects = Vec::with_capacity(capacity);

        let mut buf = vec![0u8; capacity * 12]; // 12 bytes per RoomObject
        reader.read_exact(&mut buf)?;

        for chunk in buf.chunks_exact(12) {
            let room_type = i32::from_le_bytes(chunk[0..4].try_into().unwrap());
            let x = i32::from_le_bytes(chunk[4..8].try_into().unwrap());
            let y = i32::from_le_bytes(chunk[8..12].try_into().unwrap());

            room_objects.push(RoomObject { room_type, x, y });
        }

        Ok(room_objects)
    }

    fn read_buildings<R: Read + Seek>(reader: &mut BufReader<R>) -> Result<Vec<Building>> {
        let building_count = reader.read_i32()?;
        let mut buildings = Vec::with_capacity(building_count as usize);

        for i in 0..building_count {
            let rooms_count = reader.read_i32()? as usize;

            let mut buf = vec![0u8; rooms_count * 4]; // 4 bytes per Room
            reader.read_exact(&mut buf)?;

            let room_ids = buf
                .chunks_exact(4)
                .map(|chunk| i32::from_le_bytes(chunk.try_into().expect("Chunk size should be 4")))
                .collect();

            buildings.push(Building { id: i, room_ids });
        }

        Ok(buildings)
    }

    fn read_spawns<R: Read + Seek>(
        reader: &mut BufReader<R>,
    ) -> Result<[u8; SQR_CELL_SIZE_IN_BLOCKS]> {
        let mut buf = [0u8; SQR_CELL_SIZE_IN_BLOCKS];
        reader.read_exact(&mut buf)?;

        Ok(buf)
    }
}
