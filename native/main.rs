mod buf_reader_extensions;
mod buf_writer_extensions;
mod constants;
mod lotheader;

use std::{fs, io::Result, time::Instant};

use crate::lotheader::Lotheader;

fn main() -> Result<()> {
    const B42: &str = "C:/SteamLibrary/steamapps/common/ProjectZomboidB42/media/maps/Muldraugh, KY";

    let start = Instant::now();
    let mut filescount = 0;

    for entry in fs::read_dir(B42)? {
        let path_buf = entry?.path();
        let path_str = path_buf.to_string_lossy();

        if path_buf.is_file() && path_buf.extension().and_then(|s| s.to_str()) == Some("lotheader")
        {
            // println!("{}", path_buf.file_name().unwrap().to_str().unwrap());

            Lotheader::from_file(&path_str)?;

            // println!("\tmagic:    {}", header.magic);
            // println!("\tversion:  {}", header.version);
            // println!("\ttilesdef: {}", header.tile_names.len());
            // println!("\twidth:    {}", header.width);
            // println!("\theight:   {}", header.height);
            // println!("\tmin_layer:{}", header.min_layer);
            // println!("\tmax_layer:{}", header.max_layer);

            filescount += 1;
        }
    }

    let duration = start.elapsed();

    println!(
        "{} files parsed in {}.{:.3}s ({:.3}ms / file)",
        filescount,
        duration.as_secs(),
        duration.subsec_millis(),
        duration.as_millis() as f64 / (filescount as f64)
    );

    Ok(())
}
