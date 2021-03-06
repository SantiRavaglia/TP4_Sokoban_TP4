#include "Sokoban.h"

Sokoban::Sokoban(const Nivel &n) : _mapa(n.MapaN()), _bombas(n.BombasN()),
                            _cajas(n.CajasN()), _persona(n.PersonaN()),
                            _accion() {
    int contCajasSinDepositos = 0;
    for(Coord d : n.MapaN().Depositos()) {
        bool cajaNoEnDeposito = true;
        for(Coord c : n.CajasN()) {
            if (c == d) {
                cajaNoEnDeposito = false;
            }
        }
        if (cajaNoEnDeposito) {
            contCajasSinDepositos++;
        }
    }
    _depositosSinCaja = contCajasSinDepositos;
}

Mapa Sokoban::mapa() const {
    return this->_mapa;
}

Nat Sokoban::bombas() const {
    return this->_bombas;
}

Coord Sokoban::persona() const {
    return this->_persona;
}

bool Sokoban::hayCaja(const Coord &coord) const {
    bool res = false;
    for(const Coord &c : this->_cajas){
        if(coord == c){
            res = true;
        }
    }
    return res;
}

bool Sokoban::noHayParedNiCaja(const Coord &coord) const {
    bool res = true;
    if (this->_mapa.hayPared(coord)) {
        res = false;
    }
    if (this->hayCaja(coord)) {
        res = false;
    }
    return res;
}

bool Sokoban::puedeMover(const Direccion &dir) const {
    bool res = false;
    Coord proxCord = dir.proximaCoord(this->persona());
    Coord proxProxCord = dir.proximaCoord(proxCord);
    if(!this->_mapa.hayPared(proxCord)) {
        if (hayCaja(proxCord)) {
            if(noHayParedNiCaja(proxProxCord)) {
                res = true;
            }
        } else { res = true; }
    }
    return res;
}

void Sokoban::mover(const Direccion &dir) {
    Coord proxCord = dir.proximaCoord(this->persona());
    Coord proxProxCord = dir.proximaCoord(proxCord);
    Coord posPersona = this->_persona;

    if (puedeMover(dir)) {
        if (hayCaja(proxCord)) {
            if (this->_mapa.hayDeposito(proxCord)) {
                this->_depositosSinCaja++;
                this->_cajas.erase(proxCord);
                this->_cajas.insert(proxProxCord);
                this->_persona = proxCord;
            } else if (this->_mapa.hayDeposito(proxProxCord)) {
                this->_depositosSinCaja--;
                this->_cajas.erase(proxCord);
                this->_cajas.insert(proxProxCord);
                this->_persona = proxCord;
            } else {
                this->_cajas.erase(proxCord);
                this->_cajas.insert(proxProxCord);
                this->_persona = proxCord;
            }

            this->_accion.push(make_tuple(make_tuple(true, proxCord, proxProxCord), make_tuple(false), make_tuple(true, posPersona)));
        } else {
            this->_accion.push(make_tuple(make_tuple(false, Coord(), Coord()), make_tuple(false), make_tuple(true, posPersona)));
            this->_persona = proxCord;
        }
    }
}

void Sokoban::deshacer() {
    if (!this->_accion.empty()) {
        if (get<0>(get<0>(this->_accion.top()))) {
            Coord per = Coord(get<1>(get<2>(this->_accion.top())));
            this->_persona = per;
            this->_cajas.erase(get<2>(get<0>(this->_accion.top())));
            this->_cajas.insert(get<1>(get<0>(this->_accion.top())));
        } else if (get<0>(get<1>(this->_accion.top()))) {
            this->_mapa.eliminarBomba();
            this->_bombas++;
        } else if (get<0>(get<2>(this->_accion.top()))) {
            Coord per = Coord(get<1>(get<2>(this->_accion.top())));
            this->_persona = per;
        }
        this->_accion.pop();
    }
}

void Sokoban::tirarBomba() {
    if(this->_bombas > 0) {
        this->_mapa.tirarBomba(this->_persona);
        this->_bombas--;
        this->_accion.push(make_tuple(make_tuple(false, Coord(), Coord()), make_tuple(true), make_tuple(false, this->_persona)));
    }
}

bool Sokoban::gano() const {
    return this->_depositosSinCaja == 0;
}

bool Sokoban::hayCajas(const set<Coord>& cajas) const {
    bool res = true;
    bool esta;
    for (const Coord& cajaAChequear : cajas) {
        esta = false;
        for (const Coord& cajaEstr : this->_cajas) {
            if (cajaAChequear == cajaEstr) {
                esta = true;
            }
            if (!esta) {
                res = false;
            }
        }
    }
    return res;
}
