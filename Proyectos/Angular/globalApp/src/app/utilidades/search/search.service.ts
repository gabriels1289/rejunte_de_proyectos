import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { debounceTime, distinctUntilChanged, switchMap } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})
export class SearchService {

  private terminoDeBusqueda;
  private arreglo;
  private propiedad: string;  // key o propiedad del objeto a utilizar en la busqueda

  constructor() { }

  search(terminoDeBusqueda: any, arregloDeBusqueda: any[], propiedad: string) {
    this.arreglo = arregloDeBusqueda;
    this.propiedad = propiedad;
    return terminoDeBusqueda.pipe(
      debounceTime(400),
      distinctUntilChanged(),
      switchMap(termino => this.searchEntries(termino))
      );
  }

 /*  private searchEntries(termino) {
    return this.arreglo.filter( res => res[this.propiedad] === termino );
  } */
  private searchEntries(termino) {
    const aux = this.arreglo.filter( res => res[this.propiedad].includes(termino));
    console.log('searchEntries: %o', aux);
    return aux;
  }

  search2(terminoDeBusqueda: any, arregloDeBusqueda: any[], propiedad: string) {
    this.arreglo = arregloDeBusqueda;
    this.propiedad = propiedad;
    console.log('parametros: %o', terminoDeBusqueda);
    return this.searchEntries(terminoDeBusqueda);
  }

  search3(arregloDeBusqueda: any[], propiedad: string) {
    this.arreglo = arregloDeBusqueda;
    this.propiedad = propiedad;
    console.log('parametros: %o', this.terminoDeBusqueda);
    return this.searchEntries(this.terminoDeBusqueda);
  }

  setParametro(terminoDeBusqueda: any) {
    this.terminoDeBusqueda = terminoDeBusqueda;
    /* this.arreglo = arregloDeBusqueda;
    this.propiedad = propiedad; */
  }
}
