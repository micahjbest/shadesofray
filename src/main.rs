use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::pixels::Color;
use sdl2::pixels::PixelFormatEnum;
use std::time::Instant;

use imgui;
use imgui_sdl2;
use gl;
use imgui_opengl_renderer;

pub fn main() -> Result<(), String> {
    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();

    let window = video_subsystem
        .window("rust-sdl2 demo", 800, 600)
        .position_centered()
        .opengl()
        .allow_highdpi()
        .build()
        .unwrap();

    let mut canvas = window.into_canvas().build().map_err(|e| e.to_string())?;

    let pixel_format = canvas.default_pixel_format();

    println!("pixel format: {:#?}", pixel_format);

    let texture_creator = canvas.texture_creator();
    let mut framebuffer_source = texture_creator
        .create_texture_streaming(PixelFormatEnum::ARGB8888, 800, 600)
        .map_err(|e| e.to_string())?;

    canvas.set_draw_color(Color::RGB(0, 255, 255));
    canvas.clear();
    canvas.present();

    let mut event_pump = sdl_context.event_pump().unwrap();

    'running: for frame_number in 1 as usize.. {
        let frame_start_time = Instant::now();

        let fill_start_time = Instant::now();
        let mut inner_fill_elapsed_time: f32 = 0.0;
        framebuffer_source.with_lock(None, |buffer: &mut [u8], _pitch: usize| {
            let inner_fill_start_time = Instant::now();
            let mut count: usize = 0;
            let mut offset: usize = 0;
            for y in 0..600 {
                for x in 0..800 {
                    buffer[offset] = 255;
                    buffer[offset + 1] = ((x + frame_number) % 256) as u8;
                    buffer[offset + 2] = ((y + count) % 256) as u8;
                    buffer[offset + 3] = (frame_number % 256) as u8;
                    offset += 4;
                    count += 1;
                }
            }
            inner_fill_elapsed_time = inner_fill_start_time.elapsed().as_micros() as f32 / 1_000.0;
        })?;

        let fill_elapsed_time = fill_start_time.elapsed().as_micros() as f32 / 1_000.0;

        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => {
                    canvas.clear();
                    break 'running;
                }
                _ => {}
            }
        }

        // The rest of the game loop goes here...
        canvas.clear();

        let copy_start_time = Instant::now();
        canvas.copy(&framebuffer_source, None, None)?;
        let copy_elapsed_time = copy_start_time.elapsed().as_micros() as f32 / 1_000.0;

        let present_start_time = Instant::now();
        canvas.present();
        let present_elapsed_time = present_start_time.elapsed().as_micros() as f32 / 1_000.0;

        //::std::thread::sleep(Duration::new(0, 1_000_000u32 / 60));

        let frame_elapsed_time = frame_start_time.elapsed().as_micros() as f32 / 1_000.0;

        let print_time = |name: &str, time_value: f32| {
            println!(
                "  {}: {:.2}ms ({}%)",
                name,
                time_value,
                (time_value / frame_elapsed_time) * 100.0
            );
        };

        println!("frame {} total {:.2}ms:", frame_number, frame_elapsed_time);
        print_time("fill", fill_elapsed_time);
        print_time("inner_fill", inner_fill_elapsed_time);
        print_time("copy", copy_elapsed_time);
        print_time("present", present_elapsed_time);
        println!("");
    }

    Ok(())
}
