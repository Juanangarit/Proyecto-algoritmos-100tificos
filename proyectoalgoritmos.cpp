#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

struct Estudiante {
  std::string nombre;
  int x, y;
  std::string localidad;
};

struct Salon {
  int capacidad_ideal;
  int capacidad_minima;
  int capacidad_maxima;
  std::vector<Estudiante> estudiantes_asignados;
};

struct Edificio {
  int x, y;
  std::string localidad;
  std::vector<Salon> salones;
};

double distancia(int x1, int y1, int x2, int y2) {
  return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}
bool capacidadDisponible(const Salon &salon) {
  return salon.estudiantes_asignados.size() < salon.capacidad_maxima;
}

Salon *buscarSalonConCapacidad(Edificio &edificio) {
  for (Salon &salon : edificio.salones) {
    if (capacidadDisponible(salon)) {
      return &salon;
    }
  }
  return nullptr;
}
Edificio *buscarEdificioCercano(const Estudiante &estudiante,
                                std::vector<Edificio> &edificios,
                                const std::vector<int> &edificiosEvitar = {}) {
  Edificio *edificio_mas_cercano = nullptr;
  double menor_distancia = std::numeric_limits<double>::max();

  for (int i = 0; i < edificios.size(); i++) {
    Edificio &edificio = edificios[i];

    // Verificamos si el edificio está en la lista de edificios a evitar
    if (std::find(edificiosEvitar.begin(), edificiosEvitar.end(), i) !=
        edificiosEvitar.end()) {
      continue; // Saltamos este edificio
    }

    if (edificio.localidad == estudiante.localidad) {
      double dist =
          distancia(estudiante.x, estudiante.y, edificio.x, edificio.y);
      if (dist < menor_distancia && buscarSalonConCapacidad(edificio)) {
        edificio_mas_cercano = &edificio;
        menor_distancia = dist;
      }
    }
  }
  return edificio_mas_cercano;
}

void reasignarEstudiantes(std::vector<Estudiante> &estudiantes_pendientes,
                          std::vector<Edificio> &edificios) {
  for (Estudiante &estudiante : estudiantes_pendientes) {
    Edificio *edificio_asignado = buscarEdificioCercano(estudiante, edificios);
    if (edificio_asignado) {
      Salon *salon_asignado = buscarSalonConCapacidad(*edificio_asignado);
      salon_asignado->estudiantes_asignados.push_back(estudiante);

      // Mensaje de asignación
      std::cout << "El estudiante " << estudiante.nombre
                << " ha sido asignado al edificio en "
                << edificio_asignado->localidad
                << " y al salón con capacidad ideal de "
                << salon_asignado->capacidad_ideal << ".\n";
    } else {
      // Mensaje de no disponibilidad
      std::cout << "No hay edificios disponibles para el estudiante "
                << estudiante.nombre << ".\n";
    }
  }
}

void validarEdificios(std::vector<Edificio> &edificios,
                      std::vector<Estudiante> &estudiantes_pendientes) {
  for (Edificio &edificio : edificios) {
    int totalEstudiantes = 0;
    for (const Salon &salon : edificio.salones) {
      totalEstudiantes += salon.estudiantes_asignados.size();
    }

    int capacidadTotalEdificio = 0;
    for (const Salon &salon : edificio.salones) {
      capacidadTotalEdificio += salon.capacidad_ideal;
    }

    if (totalEstudiantes <
        capacidadTotalEdificio *
            0.5) { // Suponemos 50% como umbral, pero esto puede ajustarse
      std::cout << "El edificio en " << edificio.localidad
                << " no cumple con el umbral de eficiencia.\n"
                << "Reubicando estudiantes...\n";
      for (Salon &salon : edificio.salones) {
        while (!salon.estudiantes_asignados.empty()) {
          estudiantes_pendientes.push_back(salon.estudiantes_asignados.back());
          salon.estudiantes_asignados.pop_back();
        }
      }
    }
  }

  // Una vez movidos todos los estudiantes de edificios no eficientes,
  // reasignamos
  if (!estudiantes_pendientes.empty()) {
    std::cout << "Iniciando proceso de reasignación...\n";
    reasignarEstudiantes(estudiantes_pendientes, edificios);
  } else {
    std::cout << "Todos los edificios cumplen con el umbral de eficiencia.\n";
  }
}

void ingresarEstudiante(std::vector<Estudiante> &estudiantes) {
  Estudiante estudiante;
  std::cout << "Ingresar nombre del estudiante: ";
  std::cin >> estudiante.nombre;
  std::cout << "Ingresar coordenada x de la dirección: ";
  std::cin >> estudiante.x;
  std::cout << "Ingresar coordenada y de la dirección: ";
  std::cin >> estudiante.y;
  std::cout << "Ingresar localidad del estudiante: ";
  std::cin >> estudiante.localidad;

  estudiantes.push_back(estudiante);
}

void ingresarEdificio(std::vector<Edificio> &edificios) {
  Edificio edificio;
  std::cout << "Ingresar coordenada x del edificio: ";
  std::cin >> edificio.x;
  std::cout << "Ingresar coordenada y del edificio: ";
  std::cin >> edificio.y;
  std::cout << "Ingresar localidad del edificio: ";
  std::cin >> edificio.localidad;

  int numSalones;
  std::cout << "Ingresar número de salones en el edificio: ";
  std::cin >> numSalones;
  for (int i = 0; i < numSalones; ++i) {
    Salon salon;
    std::cout << "Ingresar capacidad ideal para el salón " << i + 1 << ": ";
    std::cin >> salon.capacidad_ideal;
    std::cout << "Ingresar capacidad mínima para el salón " << i + 1 << ": ";
    std::cin >> salon.capacidad_minima;
    std::cout << "Ingresar capacidad máxima para el salón " << i + 1 << ": ";
    std::cin >> salon.capacidad_maxima;
    edificio.salones.push_back(salon);
  }

  edificios.push_back(edificio);
}

std::vector<int> edificiosPorDebajoDelUmbral(std::vector<Edificio> &edificios) {
  std::vector<int> indices;
  for (int i = 0; i < edificios.size(); i++) {
    int totalEstudiantes = 0;
    for (const Salon &salon : edificios[i].salones) {
      totalEstudiantes += salon.estudiantes_asignados.size();
    }

    int capacidadTotalEdificio = 0;
    for (const Salon &salon : edificios[i].salones) {
      capacidadTotalEdificio += salon.capacidad_ideal;
    }

    if (totalEstudiantes < capacidadTotalEdificio * 0.5) {
      indices.push_back(i);
    }
  }
  return indices;
}

void mostrarAsignaciones(const std::vector<Edificio> &edificios) {
  for (const Edificio &edificio : edificios) {
    std::cout << "Edificio en localidad: " << edificio.localidad << "\n";
    for (const Salon &salon : edificio.salones) {
      std::cout << "  Salon con capacidad ideal: " << salon.capacidad_ideal
                << "\n";
      std::cout << "  Estudiantes asignados:\n";

      if (salon.estudiantes_asignados.empty()) {
        std::cout << "    Ninguno.\n";
      } else {
        for (const Estudiante &estudiante : salon.estudiantes_asignados) {
          std::cout << "    - " << estudiante.nombre << "\n";
        }
      }
      std::cout << "\n"; // Línea adicional para mejor visualización.
    }
  }
}

int main() {
  std::vector<Edificio> edificios;
  std::vector<Estudiante> estudiantes;
  std::vector<Estudiante> estudiantes_pendientes;

  int opcion;
  do {
    std::cout << "\nMenú:\n";
    std::cout << "1. Ingresar estudiante.\n";
    std::cout << "2. Ingresar edificio.\n";
    std::cout << "3. Asignar estudiantes a edificios.\n";
    std::cout << "4. Mostrar estudiantes pendientes.\n";
    std::cout << "5. Mostrar asignaciones.\n";
    std::cout << "6. Salir.\n";
    std::cout << "Seleccione una opción: ";
    std::cin >> opcion;

    switch (opcion) {
    case 1:
      ingresarEstudiante(estudiantes);
      break;
    case 2:
      ingresarEdificio(edificios);
      break;
    case 3:
      // Asignación inicial de estudiantes a edificios:
      for (Estudiante &estudiante : estudiantes) {
        Edificio *edificio_asignado =
            buscarEdificioCercano(estudiante, edificios);
        if (edificio_asignado) {
          Salon *salon_asignado = buscarSalonConCapacidad(*edificio_asignado);
          salon_asignado->estudiantes_asignados.push_back(estudiante);
        } else {
          estudiantes_pendientes.push_back(estudiante);
        }
      }

      std::cout << "Finalizado el proceso inicial de asignación.\n";

      // Validar edificios y reasignar si es necesario
      validarEdificios(edificios, estudiantes_pendientes);
      break;
    case 4:
      std::cout << "\nEstudiantes pendientes:\n";
      for (const Estudiante &estudiante : estudiantes_pendientes) {
        std::cout << estudiante.nombre << "\n";
      }
      break;
    case 5:
      mostrarAsignaciones(edificios);
      break;
    case 6:
      std::cout << "Saliendo...\n";
      break;
    default:
      std::cout << "Opción no válida.\n";
      break;
    }

  } while (opcion != 6);

  return 0;
}

