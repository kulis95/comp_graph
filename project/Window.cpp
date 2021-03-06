#include "Window.h"
#include "SDL_function.h"

Window::Window(std::string filename) : m_width(0), m_height(0),
                                       m_filename(filename), m_screen(nullptr),
                                       m_bmp(nullptr), m_dstrect(new SDL_Rect)
{
	set_dimensions();
	init();
	create_window();
	load_image();
	//draw_image();
}

Window::Window(SDL_Surface* image) : m_width(image->w), m_height(image->h),
                                     m_filename("\0"), m_screen(nullptr),
                                     m_bmp(image), m_dstrect(new SDL_Rect)
{
	init();
	create_window();
}

Window::~Window()
{
	SDL_FreeSurface(m_screen);
	SDL_FreeSurface(m_bmp);

	delete m_screen;
	delete m_bmp;
	delete m_dstrect;

	std::cerr << "Exited with no errors." << std::endl;
	exit(EXIT_SUCCESS);
}

/** \brief Seting width and height when creating Window from file
 *
 * \return void
 *
 */
void Window::set_dimensions()
{
	std::fstream bmp_file(m_filename, std::ios_base::in | std::ios_base::binary);
	if (!bmp_file.is_open())
	{
		std::cerr << "Couldn't open file: " << m_filename << std::endl;
		exit(EXIT_FAILURE);
	}

	bmp_file.seekg(18, std::ios_base::beg); // set file to 18th byte (as 4 bytes from 18 contain witdth and 4 from 22 contain height)
	bmp_file.read(reinterpret_cast<char*>(&m_width), sizeof(m_width)); // in read first argument must be char*
	bmp_file.read(reinterpret_cast<char*>(&m_height), sizeof(m_width));
	bmp_file.close();
}

/** \brief initialize SDL video
 *
 * \return void
 *
 */
void Window::init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Unable to init SDL: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	// make sure SDL cleans up before exit
	atexit(SDL_Quit);
}

/** \brief create a new window
 *
 * \return void
 *
 */
void Window::create_window()
{
	m_screen = SDL_SetVideoMode(m_width, m_height, 16,
	                            SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (!m_screen)
	{
		std::cerr << "Unable to set " << m_width << "x" << m_height << " video: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
}

/** \brief load an image from file
 *
 * \return void
 *
 */
void Window::load_image()
{
	m_bmp = SDL::new_bmp_surface(m_filename.c_str());

	// centre the bitmap on screen
	m_dstrect->x = (m_screen->w - m_bmp->w) / 2;
	m_dstrect->y = (m_screen->h - m_bmp->h) / 2;
}

/** \brief draw image
 *
 * \return void
 *
 */
void Window::draw_image()
{
	// program main loop
	bool done = false;
	while (!done)
	{
		// message processing loop
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// check for messages
			switch (event.type)
			{
				// exit if the window is closed
			case SDL_QUIT:
				done = true;
				break;

				// check for keypresses
			case SDL_KEYDOWN:
				{
					// exit if ESCAPE is pressed
					if (event.key.keysym.sym == SDLK_ESCAPE)
						done = true;
					break;
				}
			} // end switch
		} // end of message processing

		// DRAWING STARTS HERE

		// clear screen
		SDL_FillRect(m_screen, 0, SDL_MapRGB(m_screen->format, 0, 0, 0));

		// draw bitmap
		SDL_BlitSurface(m_bmp, 0, m_screen, m_dstrect);

		// DRAWING ENDS HERE

		// finally, update the screen
		SDL_Flip(m_screen);
	} // end main loop

	// free loaded bitmap
	SDL_FreeSurface(m_bmp);
}
