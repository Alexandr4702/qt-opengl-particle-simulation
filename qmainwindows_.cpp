#include "qmainwindows_.h"

#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

Qmainwindows_::Qmainwindows_()
{
    resize(1000, 650);

    game = new GL_GAME(this);
    QWidget* control_panel = new QWidget(this);
    QVBoxLayout* controls = new QVBoxLayout(control_panel);
    QPushButton* start_pause_button = new QPushButton("Start", control_panel);
    QPushButton* reset_button = new QPushButton("Reset", control_panel);
    QLabel* particle_label = new QLabel("Particle count", control_panel);
    QSpinBox* particle_count = new QSpinBox(control_panel);
    QPushButton* spawn_button = new QPushButton("Spawn", control_panel);
    QLabel* potential_label = new QLabel("Potential", control_panel);
    QComboBox* potential = new QComboBox(control_panel);
    QLabel* time_scale_label = new QLabel("Time scale", control_panel);
    QDoubleSpinBox* time_scale = new QDoubleSpinBox(control_panel);

    particle_count->setRange(2, 4096);
    particle_count->setSingleStep(2);
    particle_count->setValue(512);
    potential->addItem("Gravity");
    potential->addItem("Coulomb");
    potential->addItem("Gravity + Coulomb");
    potential->addItem("Harmonic attraction");
    potential->addItem("Lennard-Jones");
    potential->addItem("Yukawa");
    time_scale->setRange(0.1, 10.0);
    time_scale->setSingleStep(0.1);
    time_scale->setValue(1.0);
    time_scale->setSuffix("x");

    controls->addWidget(start_pause_button);
    controls->addWidget(reset_button);
    controls->addSpacing(16);
    controls->addWidget(time_scale_label);
    controls->addWidget(time_scale);
    controls->addSpacing(16);
    controls->addWidget(potential_label);
    controls->addWidget(potential);
    controls->addSpacing(16);
    controls->addWidget(particle_label);
    controls->addWidget(particle_count);
    controls->addWidget(spawn_button);
    controls->addStretch();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(game, 4);
    layout->addWidget(control_panel, 1);

    connect(start_pause_button, &QPushButton::clicked, this,
            [this, start_pause_button]() {
                const bool running = game->toggle_simulation();
                start_pause_button->setText(running ? "Pause" : "Start");
            });
    connect(reset_button, &QPushButton::clicked, this,
            [this, start_pause_button]() {
                game->reset_simulation();
                start_pause_button->setText("Start");
            });
    connect(time_scale, &QDoubleSpinBox::valueChanged,
            game, &GL_GAME::set_time_scale);
    connect(potential, &QComboBox::currentIndexChanged, this,
            [this, start_pause_button](int index) {
                game->set_potential(index);
                start_pause_button->setText("Start");
            });
    connect(spawn_button, &QPushButton::clicked, this,
            [this, particle_count, start_pause_button]() {
                game->spawn_particles(particle_count->value());
                start_pause_button->setText("Start");
            });
}
