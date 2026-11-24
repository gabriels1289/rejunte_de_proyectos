import { Component, OnInit, Output, EventEmitter } from '@angular/core';
import { SearchService } from './search.service';
import { Subject } from 'rxjs';

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css']
})
export class SearchComponent implements OnInit {

  resultados;
  terminoDeBusqueda$ = new Subject<string>();
  terminoDeBusqueda = '';
  @Output() busca = new EventEmitter<string>();

  arreglo = [
    {
      id: 1,
      asunto: 'pepe',
      cuerpo: 'fuulano'
    },
    {
      id: 2,
      asunto: 'moni',
      cuerpo: 'fuulano'
    },
    {
      id: 3,
      asunto: 'paola',
      cuerpo: 'argento'
    },
    {
      id: 4,
      asunto: 'dardo',
      cuerpo: 'fuzeneco'
    },
    {
      id: 5,
      asunto: 'maria elena',
      cuerpo: 'fuzeneco'
    },
  ];

  constructor(
    private searchService: SearchService
  ) {
    this.searchService.search(this.terminoDeBusqueda$, this.arreglo, 'cuerpo')
      .subscribe( results => {
        console.log('resultados: %o', results);
        this.resultados = results.results;
      });
  }

  ngOnInit() {
  }

  /**
   * search
   */
  /* public search() {
    const aux = this.searchService.search2(this.terminoDeBusqueda2, this.arreglo, 'cuerpo');
    console.log('Metodo search: %o', aux);
  } */

  setParametro() {
    this.searchService.setParametro(this.terminoDeBusqueda);
  }

  buscar() {
    this.setParametro();
    console.log('terminoDeBusqueda search: %o', this.terminoDeBusqueda);
    this.busca.emit(this.terminoDeBusqueda);
  }

}
